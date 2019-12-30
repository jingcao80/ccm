//=========================================================================
// Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
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

#include "rpc/comorpc.h"
#include "rpc/binder/CBinderChannel.h"
#include "rpc/binder/CBinderParcel.h"
#include "rpc/binder/InterfacePack.h"
#include "util/comolog.h"
#include <binder/Parcel.h>
#include <binder/ProcessState.h>

namespace como {

void DeathRecipientList::Add(
    /* [in] */ DeathRecipient* recipient)
{
    Mutex::AutoLock lock(mLock);
    mDeathRecipients.Add(recipient);
}

void DeathRecipientList::Remove(
    /* [in] */ DeathRecipient* recipient)
{
    Mutex::AutoLock lock(mLock);

    Integer N = mDeathRecipients.GetSize();
    for (Integer i = 0; i < N; i++) {
        if (mDeathRecipients.Get(i) == recipient) {
            mDeathRecipients.Remove(i);
            return;
        }
    }
}

android::sp<DeathRecipient> DeathRecipientList::Find(
    /* [in] */ IDeathRecipient* recipient)
{
    Mutex::AutoLock lock(mLock);

    Integer N = mDeathRecipients.GetSize();
    for (Integer i = 0; i < N; i++) {
        if (mDeathRecipients.Get(i)->Matches(recipient)) {
            return mDeathRecipients.Get(i);
        }
    }
    return nullptr;
}

//-------------------------------------------------------------------------------------------

DeathRecipient::DeathRecipient(
    /* [in] */ IProxy* proxy,
    /* [in] */ IDeathRecipient* recipient,
    /* [in] */ DeathRecipientList* list)
    : mProxy(Object::GetWeakReference(proxy))
    , mObject(Object::GetWeakReference(recipient))
{
    mDeathRecipients->Add(this);
}

void DeathRecipient::binderDied(
    /* [in] */ const android::wp<android::IBinder>& who)
{
    AutoPtr<IDeathRecipient> recipient;
    mObject->Resolve(IID_IDeathRecipient, (IInterface**)&recipient);
    if (recipient != nullptr) {
        AutoPtr<IProxy> proxy;
        mProxy->Resolve(IID_IProxy, (IInterface**)&proxy);
        recipient->StubDied(proxy);
    }
}

void DeathRecipient::ClearReference()
{
    mDeathRecipients->Remove(this);
}

Boolean DeathRecipient::Matches(
    /* [in] */ IDeathRecipient* recipient)
{
    if (recipient == nullptr) {
        return false;
    }

    AutoPtr<IDeathRecipient> dr;
    mObject->Resolve(IID_IDeathRecipient, (IInterface**)&dr);
    if (dr == nullptr) {
        return false;
    }

    return dr == recipient;
}

//-------------------------------------------------------------------------------------------

CBinderChannel::ServiceRunnable::ServiceRunnable(
    /* [in] */ CBinderChannel* owner,
    /* [in] */ IStub* target)
    : mOwner(owner)
    , mTarget(target)
{}

android::status_t CBinderChannel::ServiceRunnable::onTransact(
    /* [in] */ uint32_t code,
    /* [in] */ const android::Parcel& data,
    /* [in] */ android::Parcel* reply,
    /* [in] */ uint32_t flags)
{
    switch(code) {
        case COMMAND_GET_COMPONENT_METADATA: {
            AutoPtr<IParcel> argParcel = new CBinderParcel(
                    const_cast<android::Parcel*>(&data), false);

            CoclassID cid;
            argParcel->ReadCoclassID(cid);
            AutoPtr<IMetaComponent> mc;
            CoGetComponentMetadata(*cid.mCid, nullptr, mc);
            Array<Byte> metadata;
            ECode ec = mc->GetSerializedMetadata(metadata);
            ReleaseCoclassID(cid);

            reply->writeInt32(NOERROR);
            reply->writeInt32(metadata.GetLength());
            reply->write(metadata.GetPayload(), metadata.GetLength());

            return android::NO_ERROR;
        }
        case COMMAND_INVOKE: {
            AutoPtr<IParcel> argParcel = new CBinderParcel(
                    const_cast<android::Parcel*>(&data), false);
            AutoPtr<IParcel> resParcel = new CBinderParcel(reply, false);
            ECode* result = (ECode*)reply->writeInplace(sizeof(ECode));
            ECode ec = mTarget->Invoke(argParcel, resParcel);
            *result = ec;

            return android::NO_ERROR;
        }
        default:
            break;
    }

    return BBinder::onTransact(code, data, reply, flags);
}

//-------------------------------------------------------------------------------------------

const CoclassID CID_CBinderChannel =
        {{0xcc32c56d,0x2cc7,0x4627,0x9594,{0x54,0xb1,0x9b,0xec,0x86,0x14}}, &CID_COMORuntime};

Boolean CBinderChannel::sThreadPoolStarted = false;
Mutex CBinderChannel::sThreadPoolLock;

COMO_INTERFACE_IMPL_1(CBinderChannel, Object, IRPCChannel);

COMO_OBJECT_IMPL(CBinderChannel);

CBinderChannel::CBinderChannel(
    /* [in] */ RPCType type,
    /* [in] */ RPCPeer peer)
    : mType(type)
    , mPeer(peer)
    , mOrgue(new DeathRecipientList())
{}

ECode CBinderChannel::Apply(
    /* [in] */ IInterfacePack* ipack)
{
    mBinder = InterfacePack::From(ipack)->GetAndroidBinder();
    return NOERROR;
}

ECode CBinderChannel::GetRPCType(
    /* [out] */ RPCType& type)
{
    type = mType;
    return NOERROR;
}

ECode CBinderChannel::IsPeerAlive(
    /* [out] */ Boolean& alive)
{
    alive = mBinder->isBinderAlive() ? true : false;
    return NOERROR;
}

ECode CBinderChannel::LinkToDeath(
    /* [in] */ IProxy* proxy,
    /* [in] */ IDeathRecipient* recipient,
    /* [in] */ HANDLE cookie,
    /* [in] */ Integer flags)
{
    if (recipient == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::sp<DeathRecipient> dr = new DeathRecipient(proxy, recipient, mOrgue);
    android::status_t st = mBinder->linkToDeath(dr, nullptr, flags);
    if (st != android::NO_ERROR) {
        dr->ClearReference();
    }

    return st == android::NO_ERROR ? NOERROR : E_FAILED_EXCEPTION;
}

ECode CBinderChannel::UnlinkToDeath(
    /* [in] */ IProxy* proxy,
    /* [in] */ IDeathRecipient* recipient,
    /* [in] */ HANDLE cookie,
    /* [in] */ Integer flags,
    /* [out] */ AutoPtr<IDeathRecipient>* outRecipient)
{
    if (recipient == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::sp<DeathRecipient> origDR = mOrgue->Find(recipient);
    android::status_t st = android::NO_ERROR;
    if (origDR != nullptr) {
        android::wp<android::IBinder::DeathRecipient> wdr;
        st = mBinder->unlinkToDeath(origDR, nullptr, flags, &wdr);
        if (st == android::NO_ERROR && wdr != nullptr) {
            android::sp<android::IBinder::DeathRecipient> sdr = wdr.promote();
            DeathRecipient* dr = static_cast<DeathRecipient*>(sdr.get());
            if (dr != nullptr) {
                dr->ClearReference();
            }
        }
    }

    return st == android::NO_ERROR ? NOERROR : E_FAILED_EXCEPTION;
}

ECode CBinderChannel::GetComponentMetadata(
    /* [in] */ const CoclassID& cid,
    /* [out, callee] */ Array<Byte>& metadata)
{
    AutoPtr<IParcel> parcel;
    CoCreateParcel(RPCType::Local, parcel);
    parcel->WriteCoclassID(cid);

    HANDLE data;
    parcel->GetPayload(data);
    android::Parcel reply;

    if (DEBUG) {
        Logger::D("CBinderChannel", "Send message.");
    }

    if (mBinder->transact(COMMAND_GET_COMPONENT_METADATA,
            *reinterpret_cast<android::Parcel*>(data), &reply) != android::NO_ERROR) {
        Logger::E("CBinderChannel", "Get component metadata failed.");
        return E_REMOTE_EXCEPTION;
    }

    ECode ec = reply.readInt32();
    if (FAILED(ec)) {
        if (DEBUG) {
            Logger::D("CBinderChannel", "Remote call failed with ec = 0x%x.", ec);
        }
        return ec;
    }

    Integer replySize = reply.readInt32();
    metadata = Array<Byte>::Allocate(replySize);
    reply.read(metadata.GetPayload(), replySize);

    return NOERROR;
}

ECode CBinderChannel::Invoke(
    /* [in] */ IMetaMethod* method,
    /* [in] */ IParcel* argParcel,
    /* [out] */ AutoPtr<IParcel>& resParcel)
{
    HANDLE data;
    argParcel->GetPayload(data);
    android::Parcel reply;

    if (DEBUG) {
        Logger::D("CBinderChannel", "Send message.");
    }

    if (mBinder->transact(COMMAND_INVOKE,
            *reinterpret_cast<android::Parcel*>(data), &reply) != android::NO_ERROR) {
        Logger::E("CBinderChannel", "Get component metadata failed.");
        return E_REMOTE_EXCEPTION;
    }

    ECode ec = reply.readInt32();
    if (FAILED(ec)) {
        if (DEBUG) {
            Logger::D("CBinderChannel", "Remote call failed with ec = 0x%x.", ec);
        }
        return ec;
    }

    Boolean hasOutArgs;
    method->HasOutArguments(hasOutArgs);
    if (hasOutArgs) {
        Long size = reply.dataAvail();
        resParcel->SetData(reinterpret_cast<HANDLE>(reply.readInplace(size)), size);
    }

    return NOERROR;
}

ECode CBinderChannel::StartListening(
    /* [in] */ IStub* stub)
{
    if (mPeer == RPCPeer::Stub) {
        mBinder = new ServiceRunnable(this, stub);

        if (sThreadPoolStarted) {
            return NOERROR;
        }

        Mutex::AutoLock lock(sThreadPoolLock);

        if (sThreadPoolStarted) {
            return NOERROR;
        }

        android::ProcessState::self()->startThreadPool();
        sThreadPoolStarted = true;
    }

    return NOERROR;
}

ECode CBinderChannel::Match(
    /* [in] */ IInterfacePack* ipack,
    /* [out] */ Boolean& matched)
{
    IBinderInterfacePack* idpack = IBinderInterfacePack::Probe(ipack);
    if (idpack != nullptr) {
        InterfacePack* pack = (InterfacePack*)idpack;
        if (pack->GetAndroidBinder() == mBinder) {
            matched = true;
            return NOERROR;
        }
    }
    matched = false;
    return NOERROR;
}

}
