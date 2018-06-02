//=========================================================================
// Copyright (C) 2018 The C++ Component Model(CCM) Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "core/AutoLock.h"
#include "core/CThread.h"
#include "core/globals.h"
#include "core/nativeapi.h"
#include "core/NativeObject.h"
#include "core/NativeRuntime.h"
#include "core/NativeRuntimeCallbacks.h"
#include "core/NativeThread.h"
#include "core/NativeThreadList.h"
#include "core/NativeTimeUtils.h"
#include "core/Thread.h"
#include <ccmlogger.h>
#include <limits.h>
#include <asm/prctl.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <signal.h>

namespace ccm {
namespace core {

Boolean NativeThread::sIsStarted = false;
pthread_key_t NativeThread::sPthreadKeySelf;
NativeConditionVariable* NativeThread::sResumeCond = nullptr;
const size_t NativeThread::kStackOverflowImplicitCheckSize = GetStackOverflowReservedBytes(kRuntimeISA);

// For implicit overflow checks we reserve an extra piece of memory at the bottom
// of the stack (lowest memory).  The higher portion of the memory
// is protected against reads and the lower is available for use while
// throwing the StackOverflow exception.
constexpr size_t kStackOverflowProtectedSize = 4 * kMemoryToolStackGuardSizeScale * KB;

static const char* kThreadNameDuringStartup = "<native thread without managed peer>";

NativeThread::NativeThread(
    /* [in] */ Boolean daemon)
    : mTls32(daemon)
    , mWaitMonitor(nullptr)
    , mInterrupted(false)
{
    mWaitMutex = new NativeMutex(String("a thread wait mutex"));
    mWaitCond = new NativeConditionVariable(
            String("a thread wait condition variable"), *mWaitMutex);
    mTlsPtr.mName = new String(kThreadNameDuringStartup);

    static_assert((sizeof(NativeThread) % 4) == 0,
                "NativeThread has a size which is not a multiple of 4.");
    mTls32.mStateAndFlags.mAsStruct.mFlags = 0;
    mTls32.mStateAndFlags.mAsStruct.mState = kNative;
    memset(&mTlsPtr.mHeldMutexes[0], 0, sizeof(mTlsPtr.mHeldMutexes));
    for (uint32_t i = 0; i < kMaxSuspendBarriers; ++i) {
        mTlsPtr.mActiveSuspendBarriers[i] = nullptr;
    }
}

void NativeThread::InitTid()
{
    mTls32.mTid = ::ccm::core::GetTid();
}

void* NativeThread::CreateCallback(
    /* [in] */ void* arg)
{
    NativeThread* self = reinterpret_cast<NativeThread*>(arg);
    NativeRuntime* runtime = NativeRuntime::Current();
    if (runtime == nullptr) {
        Logger::E("NativeThread", "Thread attaching to non-existent runtime: %s",
                self->ShortDump().string());
        return nullptr;
    }
    {
        // TODO: pass self to MutexLock - requires self to equal Thread::Current(), which is only true
        //       after self->Init().
        NativeMutex::AutoLock lock(nullptr, *Locks::sRuntimeShutdownLock);
        // Check that if we got here we cannot be shutting down (as shutdown should never have started
        // while threads are being born).
        CHECK(!runtime->IsShuttingDownLocked());
        // Note: given that the JNIEnv is created in the parent thread, the only failure point here is
        //       a mess in InitStackHwm. We do not have a reasonable way to recover from that, so abort
        //       the runtime in such a case. In case this ever changes, we need to make sure here to
        //       delete the tmp_jni_env, as we own it at this point.
        Boolean res = self->Init(runtime->GetThreadList());
        CHECK(res);
        NativeRuntime::Current()->EndThreadBirth();
    }
    {
        ScopedObjectAccess soa(self);

        // Copy peer into self, deleting global reference when done.
        CHECK(self->mTlsPtr.mPeer != nullptr);
        Thread* tPeer = reinterpret_cast<Thread*>(self->mTlsPtr.mPeer);
        self->mTlsPtr.mOPeer = reinterpret_cast<NativeObject*>(tPeer->mNativeObject);
        tPeer->Release(reinterpret_cast<HANDLE>(self));
        self->mTlsPtr.mPeer = 0;
        self->SetThreadName(tPeer->mName);

        self->SetNativePriority(tPeer->mPriority);

        runtime->GetRuntimeCallbacks()->ThreadStart(self);

        // Invoke the 'run' method of our Thread.
        tPeer->Run();
    }
    // Detach and delete self.
    NativeRuntime::Current()->GetThreadList()->Unregister(self);

    return nullptr;
}

NativeThread* NativeThread::FromManagedThread(
    /* [in] */ Thread* peer)
{
    NativeThread* result = reinterpret_cast<NativeThread*>(peer->mNative);
    return result;
}

static size_t FixStackSize(
    /* [in] */ size_t stackSize)
{
    // A stack size of zero means "use the default".
    if (stackSize == 0) {
        stackSize = NativeRuntime::Current()->GetDefaultStackSize();
    }

    // Use the bionic pthread default stack size for native threads,
    // so include that here to support apps that expect large native stacks.
    stackSize += 1 * MB;

    // It's not possible to request a stack smaller than the system-defined PTHREAD_STACK_MIN.
    if (stackSize < PTHREAD_STACK_MIN) {
        stackSize = PTHREAD_STACK_MIN;
    }

    if (NativeRuntime::Current()->ExplicitStackOverflowChecks()) {
        // It's likely that callers are trying to ensure they have at least a certain amount of
        // stack space, so we should add our reserved space on top of what they requested, rather
        // than implicitly take it away from them.
        stackSize += GetStackOverflowReservedBytes(kRuntimeISA);
    }
    else {
        // If we are going to use implicit stack checks, allocate space for the protected
        // region at the bottom of the stack.
        stackSize += NativeThread::kStackOverflowImplicitCheckSize +
                GetStackOverflowReservedBytes(kRuntimeISA);
    }

    // Some systems require the stack size to be a multiple of the system page size, so round up.
    stackSize = RoundUp(stackSize, kPageSize);

    return stackSize;
}

// Return the nearest page-aligned address below the current stack top.
static uint8_t* FindStackTop()
{
    return reinterpret_cast<uint8_t*>(
            AlignDown(__builtin_frame_address(0), kPageSize));
}

void NativeThread::InstallImplicitProtection()
{
    uint8_t* pregion = mTlsPtr.mStackBegin - kStackOverflowProtectedSize;
    // Page containing current top of stack.
    uint8_t* stackTop = FindStackTop();

    // Try to directly protect the stack.
    Logger::V("NativeThread", "installing stack protected region at %p to %p",
            static_cast<void*>(pregion), static_cast<void*>(pregion + kStackOverflowProtectedSize - 1));
    if (ProtectStack(/* fatal_on_error */ false)) {
        // Tell the kernel that we won't be needing these pages any more.
        // NB. madvise will probably write zeroes into the memory (on linux it does).
        uint32_t unwantedSize = stackTop - pregion - kPageSize;
        madvise(pregion, unwantedSize, MADV_DONTNEED);
        return;
    }

    // There is a little complexity here that deserves a special mention.  On some
    // architectures, the stack is created using a VM_GROWSDOWN flag
    // to prevent memory being allocated when it's not needed.  This flag makes the
    // kernel only allocate memory for the stack by growing down in memory.  Because we
    // want to put an mprotected region far away from that at the stack top, we need
    // to make sure the pages for the stack are mapped in before we call mprotect.
    //
    // The failed mprotect in UnprotectStack is an indication of a thread with VM_GROWSDOWN
    // with a non-mapped stack (usually only the main thread).
    //
    // We map in the stack by reading every page from the stack bottom (highest address)
    // to the stack top. (We then madvise this away.) This must be done by reading from the
    // current stack pointer downwards. Any access more than a page below the current SP
    // might cause a segv.
    // TODO: This comment may be out of date. It seems possible to speed this up. As
    //       this is normally done once in the zygote on startup, ignore for now.
    //
    // AddressSanitizer does not like the part of this functions that reads every stack page.
    // Looks a lot like an out-of-bounds access.

    // (Defensively) first remove the protection on the protected region as will want to read
    // and write it. Ignore errors.
    UnprotectStack();

    Logger::V("NativeThread", "Need to map in stack for thread at %p",
            static_cast<void*>(pregion));

    // Read every page from the high address to the low.
    volatile uint8_t dontOptimizeThis;
    for (uint8_t* p = stackTop; p >= pregion; p -= kPageSize) {
        dontOptimizeThis = *p;
    }

    Logger::V("NativeThread", "(again) installing stack protected region at %p to %p",
            static_cast<void*>(pregion), static_cast<void*>(pregion + kStackOverflowProtectedSize - 1));

    // Protect the bottom of the stack to prevent read/write to it.
    ProtectStack(/* fatal_on_error */ true);

    // Tell the kernel that we won't be needing these pages any more.
    // NB. madvise will probably write zeroes into the memory (on linux it does).
    uint32_t unwantedSize = stackTop - pregion - kPageSize;
    madvise(pregion, unwantedSize, MADV_DONTNEED);
}

ECode NativeThread::CreateNativeThread(
    /* [in] */ Thread* peer,
    /* [in] */ size_t stackSize,
    /* [in] */ Boolean daemon)
{
    CHECK(peer != nullptr);
    NativeThread* self = Current();

    Logger::V("NativeThread", "Creating native thread for %s",
            peer->mName.string());

    NativeRuntime* runtime = NativeRuntime::Current();

    // Atomically start the birth of the thread ensuring the runtime isn't shutting down.
    Boolean threadStartDuringShutdown = false;
    {
        NativeMutex::AutoLock lock(self, *Locks::sRuntimeShutdownLock);
        if (runtime->IsShuttingDownLocked()) {
            threadStartDuringShutdown = true;
        }
        else {
            runtime->StartThreadBirth();
        }
    }
    if (threadStartDuringShutdown) {
        Logger::E("NativeThread", "Thread starting during runtime shutdown");
        return E_INTERNAL_ERROR;
    }

    NativeThread* childThread = new NativeThread(daemon);
    childThread->mTlsPtr.mPeer = reinterpret_cast<HANDLE>(peer);
    peer->AddRef(reinterpret_cast<HANDLE>(childThread));
    stackSize = FixStackSize(stackSize);

    peer->mNative = reinterpret_cast<HANDLE>(childThread);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, stackSize);
    pthread_t newPthread;
    int pthreadCreateResult = pthread_create(&newPthread,
            &attr, NativeThread::CreateCallback, childThread);
    pthread_attr_destroy(&attr);

    if (pthreadCreateResult == 0) {
        return NOERROR;
    }

    {
        NativeMutex::AutoLock lock(self, *Locks::sRuntimeShutdownLock);
        runtime->EndThreadBirth();
    }
    // Manually delete the global reference since Thread::Init will not have been run.
    peer->Release(reinterpret_cast<HANDLE>(childThread));
    childThread->mTlsPtr.mPeer = 0;
    delete childThread;
    childThread = nullptr;
    // TODO: remove from thread group?
    peer->mNative = 0;
    Logger::E("NativeThread", "pthread_create (%ld stack) failed: %s",
            stackSize, strerror(pthreadCreateResult));
    return E_OUT_OF_MEMORY_ERROR;
}

Boolean NativeThread::Init(
    /* [in] */ NativeThreadList* threadList)
{
    // This function does all the initialization that must be run by the native thread it applies to.
    // (When we create a new thread from managed code, we allocate the Thread* in Thread::Create so
    // we can handshake with the corresponding native thread when it's ready.) Check this native
    // thread hasn't been through here already...
    CHECK(NativeThread::Current() == nullptr);

    // Set pthread_self_ ahead of pthread_setspecific, that makes Thread::Current function, this
    // avoids pthread_self_ ever being invalid when discovered from Thread::Current().
    mTlsPtr.mPthreadSelf = pthread_self();
    CHECK(mIsStarted);

    SetUpAlternateSignalStack();
    if (!InitStackHwm()) {
        return false;
    }
    InitCpu();
    RemoveSuspendTrigger();
    InitTid();

#ifdef ART_TARGET_ANDROID
    __get_tls()[TLS_SLOT_ART_THREAD_SELF] = this;
#else
    pthread_setspecific(NativeThread::sPthreadKeySelf, this);
#endif
    CHECK(NativeThread::Current() == this);

    mTls32.mThinLockThreadId = threadList->AllocThreadId(this);

    threadList->Register(this);
    return true;
}

static void arch_prctl(int code, void* val)
{
    syscall(__NR_arch_prctl, code, val);
}

#if defined(__x86_64__)
void NativeThread::InitCpu()
{
    NativeMutex::AutoLock lock(nullptr, *Locks::sModifyLdtLock);

    arch_prctl(ARCH_SET_GS, this);

    // Allow easy indirection back to Thread*.
    mTlsPtr.mSelf = this;
}
#endif

NativeThread* NativeThread::Current()
{
    if (!sIsStarted) {
        return nullptr;
    }
    else {
        void* thread = pthread_getspecific(NativeThread::sPthreadKeySelf);
        return reinterpret_cast<NativeThread*>(thread);
    }
}

template <typename PeerAction>
NativeThread* NativeThread::Attach(
    /* [in] */ const String& threadName,
    /* [in] */ Boolean asDaemon,
    /* [in] */ PeerAction peerAction)
{
    NativeRuntime* runtime = NativeRuntime::Current();
    if (runtime == nullptr) {
        Logger::E("NativeThread", "Thread attaching to non-existent runtime: %s",
                threadName.string());
        return nullptr;
    }
    NativeThread* self;
    {
        NativeMutex::AutoLock lock(nullptr, *Locks::sRuntimeShutdownLock);
        if (runtime->IsShuttingDownLocked()) {
            Logger::W("NativeThread", "Thread attaching while runtime is shutting down: %s",
                    threadName.string());
            return nullptr;
        }
        else {
            NativeRuntime::Current()->StartThreadBirth();
            self = new NativeThread(asDaemon);
            Boolean initSuccess = self->Init(runtime->GetThreadList());
            NativeRuntime::Current()->EndThreadBirth();
            if (!initSuccess) {
                delete self;
                return nullptr;
            }
        }
    }

    CHECK(self->GetState() != kRunnable);
    self->SetState(kNative);

    // Run the action that is acting on the peer.
    if (!peerAction(self)) {
        runtime->GetThreadList()->Unregister(self);
        // Unregister deletes self, no need to do this here.
        return nullptr;
    }

    if (DEBUG) {
        if (!threadName.IsNull()) {
            Logger::V("NativeThread", "Attaching thread %s", threadName.string());
        }
        else {
            Logger::V("NativeThread", "Attaching unnamed thread.");
        }
    }

    {
        ScopedObjectAccess soa(self);
        runtime->GetRuntimeCallbacks()->ThreadStart(self);
    }

    return self;
}

NativeThread* NativeThread::Attach(
    /* [in] */ const String& threadName,
    /* [in] */ Boolean asDaemon,
    /* [in] */ IThreadGroup* threadGroup,
    /* [in] */ Boolean createPeer)
{
    auto createPeerAction = [&](NativeThread* self) {
        if (createPeer) {
            ECode ec = self->CreatePeer(threadName, asDaemon, threadGroup);
            if (FAILED(ec)) {
                Logger::E("NativeThread", "Exception creating thread peer:0x%08x", ec);
                return false;
            }
        }
        else {
            // These aren't necessary, but they improve diagnostics for unit tests & command-line tools.
            if (!threadName.IsNull()) {
                *self->mTlsPtr.mName = threadName;
                ::ccm::core::SetThreadName(threadName);
            }
            else {
                Logger::W("NativeThread", "%s attached without supplying a name",
                        NativeThread::Current()->ShortDump().string());
            }
        }
        return true;
    };
    return Attach(threadName, asDaemon, createPeerAction);
}

ECode NativeThread::CreatePeer(
    /* [in] */ const String& name,
    /* [in] */ Boolean asDaemon,
    /* [in] */ IThreadGroup* threadGroup)
{
    NativeRuntime* runtime = NativeRuntime::Current();
    CHECK(runtime->IsStarted());
    if (threadGroup == nullptr) {
        threadGroup = runtime->GetMainThreadGroup();
    }
    Integer threadPriority = GetNativePriority();
    Boolean threadIsDaemon = asDaemon;

    AutoPtr<IThread> peer;
    CThread::New(reinterpret_cast<HANDLE>(this), IID_IThread, (IInterface**)&peer);
    {
        ScopedObjectAccess soa(this);
        mTlsPtr.mOPeer = reinterpret_cast<NativeObject*>(
                Thread::From(peer)->mNativeObject);
    }
    ECode ec = Thread::From(peer)->constructor(threadGroup, name, threadPriority, threadIsDaemon);
    if (FAILED(ec)) {
        return ec;
    }
    threadGroup->Add(peer);

    NativeThread* self = this;
    CHECK(self == NativeThread::Current());
    ((Thread*)peer.Get())->mNative = reinterpret_cast<HANDLE>(self);

    ScopedObjectAccess soa(self);
    String peerThreadName = GetThreadName();
    SetThreadName(peerThreadName);

    return NOERROR;
}

void NativeThread::SetThreadName(
    /* [in] */ const String& name)
{
    *mTlsPtr.mName = name;
    ::ccm::core::SetThreadName(name);
}

Thread* NativeThread::GetPeerThread() const
{
    Thread* tPeer = (Thread*)reinterpret_cast<SyncObject*>(
            GetPeer()->mCcmObject);
    return tPeer;
}

static void GetThreadStack(
    /* [in] */ pthread_t thread,
    /* [in] */ void** stackBase,
    /* [in] */ size_t* stackSize,
    /* [in] */ size_t* guardSize)
{
    pthread_attr_t attributes;
    pthread_getattr_np(thread, &attributes);
    pthread_attr_getstack(&attributes, stackBase, stackSize);
    pthread_attr_getguardsize(&attributes, guardSize);
    pthread_attr_destroy(&attributes);
}

Boolean NativeThread::InitStackHwm()
{
    void* readStackBase;
    size_t readStackSize;
    size_t readGuardSize;
    GetThreadStack(mTlsPtr.mPthreadSelf, &readStackBase, &readStackSize, &readGuardSize);

    mTlsPtr.mStackBegin = reinterpret_cast<uint8_t*>(readStackBase);
    mTlsPtr.mStackSize = readStackSize;
    // The minimum stack size we can cope with is the overflow reserved bytes (typically
    // 8K) + the protected region size (4K) + another page (4K).  Typically this will
    // be 8+4+4 = 16K.  The thread won't be able to do much with this stack even the GC takes
    // between 8K and 12K.
    uint32_t minStack = GetStackOverflowReservedBytes(kRuntimeISA) + kStackOverflowProtectedSize
            + 4 * KB;
    if (readStackSize <= minStack) {
        Logger::E("NativeThread", "Attempt to attach a thread with a too-small stack");
        return false;
    }

    Logger::V("NativeThread", "Native stack is at %p (%lu with %lu guard)",
            readStackBase, readStackSize, readGuardSize);

    // Set stack_end_ to the bottom of the stack saving space of stack overflows

    NativeRuntime* runtime = NativeRuntime::Current();
    Boolean implicitStackCheck = !runtime->ExplicitStackOverflowChecks();

    ResetDefaultStackEnd();

    // Install the protected region if we are doing implicit overflow checks.
    if (implicitStackCheck) {
        // The thread might have protected region at the bottom.  We need
        // to install our own region so we need to move the limits
        // of the stack to make room for it.

        mTlsPtr.mStackBegin += readGuardSize + kStackOverflowProtectedSize;
        mTlsPtr.mStackEnd += readGuardSize + kStackOverflowProtectedSize;
        mTlsPtr.mStackSize -= readGuardSize;

        InstallImplicitProtection();
    }

    // Sanity check.
    CHECK(FindStackTop() > reinterpret_cast<void*>(mTlsPtr.mStackEnd));

    return true;
}

String NativeThread::ShortDump() const
{
    return String();
}

String NativeThread::GetThreadName()
{
    if (mTlsPtr.mOPeer == nullptr) {
        return String();
    }
    Thread* tPeer = (Thread*)reinterpret_cast<SyncObject*>(
            mTlsPtr.mOPeer->mCcmObject);
    return tPeer->mName;
}

void NativeThread::GetThreadName(
    /* [in] */ String* name)
{
    *name = *mTlsPtr.mName;
}

NativeThreadState NativeThread::SetState(
    /* [in] */ NativeThreadState newState)
{
    // Should only be used to change between suspended states.
    // Cannot use this code to change into or from Runnable as changing to Runnable should
    // fail if old_state_and_flags.suspend_request is true and changing from Runnable might
    // miss passing an active suspend barrier.
    CHECK(newState != kRunnable);
    union StateAndFlags oldStateAndFlags;
    oldStateAndFlags.mAsInt = mTls32.mStateAndFlags.mAsInt;
    CHECK(oldStateAndFlags.mAsStruct.mState != kRunnable);
    mTls32.mStateAndFlags.mAsStruct.mState = newState;
    return static_cast<NativeThreadState>(oldStateAndFlags.mAsStruct.mState);
}

Boolean NativeThread::IsSuspended() const
{
    union StateAndFlags stateAndFlags;
    stateAndFlags.mAsInt = mTls32.mStateAndFlags.mAsInt;
    return stateAndFlags.mAsStruct.mState != kRunnable &&
            (stateAndFlags.mAsStruct.mFlags & kSuspendRequest) != 0;
}

Boolean NativeThread::ModifySuspendCount(
    /* [in] */ NativeThread* self,
    /* [in] */ Integer delta,
    /* [in] */ AtomicInteger* suspendBarrier,
    /* [in] */ Boolean forDebugger)
{
    if (delta > 0 && suspendBarrier != nullptr) {
        // When delta > 0 (requesting a suspend), ModifySuspendCountInternal() may fail either if
        // active_suspend_barriers is full or we are in the middle of a thread flip. Retry in a loop.
        while (true) {
            if (LIKELY(ModifySuspendCountInternal(self, delta, suspendBarrier, forDebugger))) {
                return true;
            }
            else {
                // Failure means the list of active_suspend_barriers is full or we are in the middle of a
                // thread flip, we should release the thread_suspend_count_lock_ (to avoid deadlock) and
                // wait till the target thread has executed or Thread::PassActiveSuspendBarriers() or the
                // flip function. Note that we could not simply wait for the thread to change to a suspended
                // state, because it might need to run checkpoint function before the state change or
                // resumes from the resume_cond_, which also needs thread_suspend_count_lock_.
                //
                // The list of active_suspend_barriers is very unlikely to be full since more than
                // kMaxSuspendBarriers threads need to execute SuspendAllInternal() simultaneously, and
                // target thread stays in kRunnable in the mean time.
                Locks::sThreadSuspendCountLock->ExclusiveUnlock(self);
                NanoSleep(100000);
                Locks::sThreadSuspendCountLock->ExclusiveLock(self);
            }
        }
    }
    else {
        return ModifySuspendCountInternal(self, delta, suspendBarrier, forDebugger);
    }
}

void NativeThread::TransitionToSuspendedAndRunCheckpoints(
    /* [in] */ NativeThreadState newState)
{
    CHECK(newState != kRunnable);
    CHECK(GetState() == kRunnable);
    union StateAndFlags oldStateAndFlags;
    union StateAndFlags newStateAndFlags;
    while (true) {
        oldStateAndFlags.mAsInt = mTls32.mStateAndFlags.mAsInt;
        newStateAndFlags.mAsStruct.mFlags = oldStateAndFlags.mAsStruct.mFlags;
        newStateAndFlags.mAsStruct.mState = newState;
        // CAS the value with a memory ordering.
        Boolean done = mTls32.mStateAndFlags.mAsAtomicInt.CompareExchangeWeakRelease(
                oldStateAndFlags.mAsInt, newStateAndFlags.mAsInt);
        if (LIKELY(done)) {
            break;
        }
    }
}

// Attempt to rectify locks so that we dump thread list with required locks before exiting.
static void UnsafeLogFatalForSuspendCount(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeThread* thread)
{}

Boolean NativeThread::ModifySuspendCountInternal(
    /* [in] */ NativeThread* self,
    /* [in] */ Integer delta,
    /* [in] */ AtomicInteger* suspendBarrier,
    /* [in] */ Boolean forDebugger)
{
    if (UNLIKELY(delta < 0 && mTls32.mSuspendCount <= 0)) {
        UnsafeLogFatalForSuspendCount(self, this);
        return false;
    }

    uint16_t flags = kSuspendRequest;
    if (delta > 0 && suspendBarrier != nullptr) {
        uint32_t availableBarrier = kMaxSuspendBarriers;
        for (uint32_t i = 0; i < kMaxSuspendBarriers; ++i) {
            if (mTlsPtr.mActiveSuspendBarriers[i] == nullptr) {
                availableBarrier = i;
                break;
            }
        }
        if (availableBarrier == kMaxSuspendBarriers) {
            // No barrier spaces available, we can't add another.
            return false;
        }
        mTlsPtr.mActiveSuspendBarriers[availableBarrier] = suspendBarrier;
        flags |= kActiveSuspendBarrier;
    }

    mTls32.mSuspendCount += delta;
    if (forDebugger) {
        mTls32.mDebugSuspendCount += delta;
    }

    if (mTls32.mSuspendCount == 0) {
        AtomicClearFlag(kSuspendRequest);
    }
    else {
        // Two bits might be set simultaneously.
        mTls32.mStateAndFlags.mAsAtomicInt.FetchAndOrSequentiallyConsistent(flags);
        TriggerSuspend();
    }
    return true;
}

void NativeThread::PassActiveSuspendBarriers()
{
    while (true) {
        uint16_t currentFlags = mTls32.mStateAndFlags.mAsStruct.mFlags;
        if (LIKELY((currentFlags & kActiveSuspendBarrier) == 0)) {
            break;
        }
        else {
            PassActiveSuspendBarriers(this);
        }
    }
}

Boolean NativeThread::PassActiveSuspendBarriers(
    /* [in] */ NativeThread* self)
{
    // Grab the suspend_count lock and copy the current set of
    // barriers. Then clear the list and the flag. The ModifySuspendCount
    // function requires the lock so we prevent a race between setting
    // the kActiveSuspendBarrier flag and clearing it.
    AtomicInteger* passBarriers[kMaxSuspendBarriers];
    {
        NativeMutex::AutoLock lock(self, *Locks::sThreadSuspendCountLock);
        if (!ReadFlag(kActiveSuspendBarrier)) {
            // quick exit test: the barriers have already been claimed - this is
            // possible as there may be a race to claim and it doesn't matter
            // who wins.
            // All of the callers of this function (except the SuspendAllInternal)
            // will first test the kActiveSuspendBarrier flag without lock. Here
            // double-check whether the barrier has been passed with the
            // suspend_count lock.
            return false;
        }

        for (uint32_t i = 0; i < kMaxSuspendBarriers; ++i) {
            passBarriers[i] = mTlsPtr.mActiveSuspendBarriers[i];
            mTlsPtr.mActiveSuspendBarriers[i] = nullptr;
        }
        AtomicClearFlag(kActiveSuspendBarrier);
    }

    uint32_t barrierCount = 0;
    for (uint32_t i = 0; i < kMaxSuspendBarriers; i++) {
        AtomicInteger* pendingThreads = passBarriers[i];
        if (pendingThreads != nullptr) {
            Boolean done = false;
            do {
                int32_t curVal = pendingThreads->LoadRelaxed();
                CHECK(curVal > 0);
                // Reduce value by 1.
                done = pendingThreads->CompareExchangeWeakRelaxed(curVal, curVal - 1);
                if (done && (curVal - 1) == 0) {  // Weak CAS may fail spuriously.
                    futex(pendingThreads->Address(), FUTEX_WAKE, -1, nullptr, nullptr, 0);
                }
            } while (!done);
            ++barrierCount;
        }
    }
    CHECK(barrierCount > 0);
    return true;
}

void NativeThread::TransitionFromRunnableToSuspended(
    /* [in] */ NativeThreadState newState)
{
    CHECK(this == Current());
    // Change to non-runnable state, thereby appearing suspended to the system.
    TransitionToSuspendedAndRunCheckpoints(newState);
    // Mark the release of the share of the mutator_lock_.
    Locks::sMutatorLock->TransitionFromRunnableToSuspended(this);
    // Once suspended - check the active suspend barrier flag
    PassActiveSuspendBarriers();
}

NativeThreadState NativeThread::TransitionFromSuspendedToRunnable()
{
    union StateAndFlags oldStateAndFlags;
    oldStateAndFlags.mAsInt = mTls32.mStateAndFlags.mAsInt;
    int16_t oldState = oldStateAndFlags.mAsStruct.mState;
    CHECK(static_cast<NativeThreadState>(oldState) != kRunnable);
    do {
        Locks::sMutatorLock->AssertNotHeld(this);  // Otherwise we starve GC..
        oldStateAndFlags.mAsInt = mTls32.mStateAndFlags.mAsInt;
        CHECK(oldStateAndFlags.mAsStruct.mState == oldState);
        if (LIKELY(oldStateAndFlags.mAsStruct.mFlags == 0)) {
            // Optimize for the return from native code case - this is the fast path.
            // Atomically change from suspended to runnable if no suspend request pending.
            union StateAndFlags newStateAndFlags;
            newStateAndFlags.mAsInt = oldStateAndFlags.mAsInt;
            newStateAndFlags.mAsStruct.mState = kRunnable;
            // CAS the value with a memory barrier.
            if (LIKELY(mTls32.mStateAndFlags.mAsAtomicInt.CompareExchangeWeakAcquire(
                    oldStateAndFlags.mAsInt, newStateAndFlags.mAsInt))) {
                // Mark the acquisition of a share of the mutator_lock_.
                Locks::sMutatorLock->TransitionFromSuspendedToRunnable(this);
                break;
            }
        }
        else if ((oldStateAndFlags.mAsStruct.mFlags & kActiveSuspendBarrier) != 0) {
            PassActiveSuspendBarriers(this);
        }
        else if ((oldStateAndFlags.mAsStruct.mFlags & kSuspendRequest) != 0) {
            // Wait while our suspend count is non-zero.

            // We pass null to the MutexLock as we may be in a situation where the
            // runtime is shutting down. Guarding ourselves from that situation
            // requires to take the shutdown lock, which is undesirable here.
            NativeThread* threadToPass = nullptr;
            NativeMutex::AutoLock lock(threadToPass, *Locks::sThreadSuspendCountLock);
            oldStateAndFlags.mAsInt = mTls32.mStateAndFlags.mAsInt;
            CHECK(oldStateAndFlags.mAsStruct.mState == oldState);
            while ((oldStateAndFlags.mAsStruct.mFlags & kSuspendRequest) != 0) {
                // Re-check when Thread::resume_cond_ is notified.
                NativeThread::sResumeCond->Wait(threadToPass);
                oldStateAndFlags.mAsInt = mTls32.mStateAndFlags.mAsInt;
                CHECK(oldStateAndFlags.mAsStruct.mState == oldState);
            }
            CHECK(GetSuspendCount() == 0);
        }
    } while (true);
    return static_cast<NativeThreadState>(oldState);
}

void NativeThread::ThreadExitCallback(
    /* [in] */ void* arg)
{
    NativeThread* self = reinterpret_cast<NativeThread*>(arg);
    if (self->mTls32.mThreadExitCheckCount == 0) {
        Logger::W("NativeThread", "Native thread exiting without having called DetachCurrentThread (maybe it's "
                "going to use a pthread_key_create destructor?): %s", self->ShortDump().string());
        CHECK(sIsStarted);
        pthread_setspecific(NativeThread::sPthreadKeySelf, self);
        self->mTls32.mThreadExitCheckCount = 1;
    }
    else {
        Logger::E("NativeThread", "Native thread exited without calling DetachCurrentThread: %s",
                self->ShortDump().string());
    }
}

void NativeThread::Startup()
{
    CHECK(!sIsStarted);
    sIsStarted = true;

    pthread_key_create(&sPthreadKeySelf, NativeThread::ThreadExitCallback);

    if (pthread_getspecific(sPthreadKeySelf) != nullptr) {
        Logger::E("NativeThread", "Newly created pthread TLS slot is not nullptr");
    }
}

void NativeThread::FinishStartup()
{
    NativeRuntime* runtime = NativeRuntime::Current();
    CHECK(runtime->IsStarted());

    // Finish attaching the main thread.
    ScopedObjectAccess soa(NativeThread::Current());
    NativeThread::Current()->CreatePeer(String("main"), false, runtime->GetMainThreadGroup());
}

void NativeThread::Destroy()
{
    NativeThread* self = this;
    CHECK(self == NativeThread::Current());

    if (mTlsPtr.mPeer != 0) {
        Thread* tPeer = reinterpret_cast<Thread*>(mTlsPtr.mPeer);
        tPeer->Release(reinterpret_cast<HANDLE>(this));
        mTlsPtr.mPeer = 0;
    }

    if (mTlsPtr.mOPeer != nullptr) {
        ScopedObjectAccess soa(self);
        // We may need to call user-supplied managed code, do this before final clean-up.
        RemoveFromThreadGroup(soa);

        Thread* tPeer = (Thread*)reinterpret_cast<SyncObject*>(
                mTlsPtr.mOPeer->mCcmObject);
        tPeer->mNative = 0;
        NativeRuntime* runtime = NativeRuntime::Current();
        if (runtime != nullptr) {
            runtime->GetRuntimeCallbacks()->ThreadDeath(self);
        }

        // Thread.join() is implemented as an Object.wait() on the Thread.lock object. Signal anyone
        // who is waiting.
        SyncObject* lock = tPeer->mLock;
        if (lock != nullptr) {
            AutoLock locker(lock);
            lock->NotifyAll();
        }

        mTlsPtr.mOPeer = nullptr;
    }
}

void NativeThread::RemoveFromThreadGroup(
    /* [in] */ ScopedObjectAccess& soa)
{
    Thread* tPeer = (Thread*)reinterpret_cast<SyncObject*>(
            mTlsPtr.mOPeer->mCcmObject);
    IThreadGroup* group = tPeer->mGroup;
    if (group != nullptr) {
        ScopedThreadStateChange tsc(soa.Self(), kNative);
        group->ThreadTerminated(tPeer);
    }
}

#if defined(__x86_64__)

static void SigAltStack(stack_t* newStack, stack_t* oldStack)
{
    if (sigaltstack(newStack, oldStack) == -1) {
        Logger::E("NativeThread", "sigaltstack failed");
    }
}

// The default SIGSTKSZ on linux is 8K.  If we do any logging in a signal
// handler or do a stack unwind, this is too small.  We allocate 32K
// instead of the minimum signal stack size.
// TODO: We shouldn't do logging (with locks) in signal handlers.
static constexpr int kHostAltSigStackSize =
        32 * KB < MINSIGSTKSZ ? MINSIGSTKSZ : 32 * KB;

void NativeThread::SetUpAlternateSignalStack()
{
    // Create and set an alternate signal stack.
    stack_t ss;
    ss.ss_sp = new uint8_t[kHostAltSigStackSize];
    ss.ss_size = kHostAltSigStackSize;
    ss.ss_flags = 0;
    CHECK(ss.ss_sp != nullptr);
    SigAltStack(&ss, nullptr);

    // Double-check that it worked.
    ss.ss_sp = nullptr;
    SigAltStack(nullptr, &ss);
    Logger::V("NativeThread", "Alternate signal stack is %lu at %p",
            ss.ss_size, ss.ss_sp);
}

#endif

void NativeThread::Notify()
{
    NativeThread* self = Current();
    NativeMutex::AutoLock lock(self, *mWaitMutex);
    NotifyLocked(self);
}

void NativeThread::NotifyLocked(
    /* [in] */ NativeThread* self)
{
    if (mWaitMonitor != nullptr) {
        mWaitCond->Signal(self);
    }
}

void NativeThread::SetHeldMutex(
    /* [in] */ LockLevel level,
    /* [in] */ BaseMutex* mutex)
{
    mTlsPtr.mHeldMutexes[level] = mutex;
}

#if defined(__x86_64__)
void NativeThread::SetNativePriority(
    /* [in] */ int newPriority)
{
  // Do nothing.
}

int NativeThread::GetNativePriority()
{
    return kNormThreadPriority;
}

#endif

Boolean NativeThread::ProtectStack(
    /* [in] */ Boolean fatalOnError)
{
    void* pregion = mTlsPtr.mStackBegin - kStackOverflowProtectedSize;
    Logger::V("NativeThread", "Protecting stack at %p", pregion);
    if (mprotect(pregion, kStackOverflowProtectedSize, PROT_NONE) == -1) {
        if (fatalOnError) {
            Logger::E("NativeThread", "Unable to create protected region in stack for implicit overflow check. "
                    "Reason: %s size: %lu", strerror(errno), kStackOverflowProtectedSize);
        }
        return false;
    }
    return true;
}

Boolean NativeThread::UnprotectStack()
{
    void* pregion = mTlsPtr.mStackBegin - kStackOverflowProtectedSize;
    Logger::V("NativeThread", "Unprotecting stack at %p", pregion);
    return mprotect(pregion, kStackOverflowProtectedSize, PROT_READ | PROT_WRITE) == 0;
}

}
}
