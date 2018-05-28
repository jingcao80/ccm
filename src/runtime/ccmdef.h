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

#ifndef __CCM_CCMDEF_H__
#define __CCM_CCMDEF_H__

#if defined(_DEBUG)
#include <assert.h>
#endif
#include <stddef.h>
#include <stdint.h>

namespace ccm {

#define COCLASS_ID(x)
#define INTERFACE_ID(x)

#define interface       struct

#define Coclass(name)      class name : public _##name

#ifndef EXTERN_C
#define EXTERN_C        extern "C"
#endif

#define COM_PUBLIC      __attribute__ ((visibility ("default")))
#define COM_LOCAL       __attribute__ ((visibility ("hidden")))

#define INIT_PROI_1 __attribute__ ((init_priority (500)))
#define INIT_PROI_2 __attribute__ ((init_priority (1000)))
#define INIT_PROI_3 __attribute__ ((init_priority (1500)))
#define INIT_PROI_4 __attribute__ ((init_priority (2000)))
#define INIT_PROI_5 __attribute__ ((init_priority (2500)))
#define INIT_PROI_6 __attribute__ ((init_priority (3000)))
#define INIT_PROI_7 __attribute__ ((init_priority (3500)))
#define INIT_PROI_8 __attribute__ ((init_priority (4000)))
#define INIT_PROI_9 __attribute__ ((init_priority (4500)))
#define INIT_PROI_10 __attribute__ ((init_priority (5000)))

#define CONS_PROI_1 __attribute__ ((constructor (500)))
#define CONS_PROI_2 __attribute__ ((constructor (1000)))
#define CONS_PROI_3 __attribute__ ((constructor (1500)))
#define CONS_PROI_4 __attribute__ ((constructor (2000)))
#define CONS_PROI_5 __attribute__ ((constructor (2500)))
#define CONS_PROI_6 __attribute__ ((constructor (3000)))
#define CONS_PROI_7 __attribute__ ((constructor (3500)))
#define CONS_PROI_8 __attribute__ ((constructor (4000)))
#define CONS_PROI_9 __attribute__ ((constructor (4500)))
#define CONS_PROI_10 __attribute__ ((constructor (5000)))

#define DEST_PROI_1 __attribute__ ((destructor (500)))
#define DEST_PROI_2 __attribute__ ((destructor (1000)))
#define DEST_PROI_3 __attribute__ ((destructor (1500)))
#define DEST_PROI_4 __attribute__ ((destructor (2000)))
#define DEST_PROI_5 __attribute__ ((destructor (2500)))
#define DEST_PROI_6 __attribute__ ((destructor (3000)))
#define DEST_PROI_7 __attribute__ ((destructor (3500)))
#define DEST_PROI_8 __attribute__ ((destructor (4000)))
#define DEST_PROI_9 __attribute__ ((destructor (4500)))
#define DEST_PROI_10 __attribute__ ((destructor (5000)))

#define REFCOUNT_ADD(i)     if (i) { (i)->AddRef(); }
#define REFCOUNT_RELEASE(i) if (i) { (i)->Release(); }

#define VALIDATE_NOT_NULL(i) \
        if (i == nullptr) { return E_ILLEGAL_ARGUMENT_EXCEPTION; }

#ifndef MAX
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b)       (((a) < (b)) ? (a) : (b))
#endif

#define ALIGN4(v) (((v) + 3) & ~3)
#define ALIGN8(v) (((v) + 7) & ~7)

#if defined(__i386__)
#define ALIGN(v) ALIGN4(v)
#elif defined(__x86_64__)
#define ALIGN(v) ALIGN8(v)
#endif

template<typename T>
constexpr T RoundDown(T x, size_t n)
{
    return x & ~(n - 1);
}

template<typename T>
constexpr T RoundUp(T x, size_t n)
{
    return RoundDown(x + n - 1, n);
}

template<typename T>
inline T* AlignDown(T* x, uintptr_t n)
{
    return reinterpret_cast<T*>(RoundDown(reinterpret_cast<uintptr_t>(x), n));
}

#ifndef CHECK
#if defined(_DEBUG)
#define CHECK(e)    assert(e)
#else
#define CHECK(e)
#endif
#endif

#ifndef PACKED
#define PACKED(x) __attribute__ ((__aligned__(x), __packed__))
#endif

#ifdef __GNUC__
#define LIKELY(x)       __builtin_expect(!!(x), 1)
#define UNLIKELY(x)     __builtin_expect(!!(x), 0)
#else
#define LIKELY(x)       (x)
#define UNLIKELY(x)     (x)
#endif

#ifndef CCM_INTERFACE_DECL
#define CCM_INTERFACE_DECL()                            \
    Integer AddRef(                                     \
        /* [in] */ HANDLE id = 0) override;             \
                                                        \
    Integer Release(                                    \
        /* [in] */ HANDLE id = 0) override;             \
                                                        \
    IInterface* Probe(                                  \
        /* [in] */ const InterfaceID& iid) override;    \
                                                        \
    ECode GetInterfaceID(                               \
        /* [in] */ IInterface* object,                  \
        /* [out] */ InterfaceID* iid) override;
#endif

#ifndef CCM_INTERFACE_IMPL_1
#define CCM_INTERFACE_IMPL_1(ClassName, SupperClassName, InterfaceName)       \
    Integer ClassName::AddRef(                             \
        /* [in] */ HANDLE id)                              \
    {                                                      \
        return SupperClassName::AddRef(id);                \
    }                                                      \
                                                           \
    Integer ClassName::Release(                            \
        /* [in] */ HANDLE id)                              \
    {                                                      \
        return SupperClassName::Release(id);               \
    }                                                      \
                                                           \
    IInterface* ClassName::Probe(                          \
        /* [in] */ const InterfaceID& iid)                 \
    {                                                      \
        if (iid == IID_IInterface) {                       \
            return (IInterface*)(InterfaceName*)this;      \
        }                                                  \
        else if (iid == IID_##InterfaceName) {             \
            return (InterfaceName*)this;                   \
        }                                                  \
        return SupperClassName::Probe(iid);                \
    }                                                      \
                                                           \
    ECode ClassName::GetInterfaceID(                       \
        /* [in] */ IInterface* object,                     \
        /* [out] */ InterfaceID* iid)                      \
    {                                                      \
        VALIDATE_NOT_NULL(iid);                            \
                                                           \
        if (object == (IInterface*)(InterfaceName*)this) { \
            *iid = IID_##InterfaceName;                    \
            return NOERROR;                                \
        }                                                  \
        return SupperClassName::GetInterfaceID(object, iid); \
    }
#endif

#ifndef CCM_INTERFACE_IMPL_2
#define CCM_INTERFACE_IMPL_2(ClassName, SupperClassName, Interface1, Interface2) \
    Integer ClassName::AddRef(                                  \
        /* [in] */ HANDLE id)                                   \
    {                                                           \
        return SupperClassName::AddRef(id);                     \
    }                                                           \
                                                                \
    Integer ClassName::Release(                                 \
        /* [in] */ HANDLE id)                                   \
    {                                                           \
        return SupperClassName::Release(id);                    \
    }                                                           \
                                                                \
    IInterface* ClassName::Probe(                               \
        /* [in] */ const InterfaceID& iid)                      \
    {                                                           \
        if (iid == IID_IInterface) {                            \
            return (IInterface*)(Interface1*)this;              \
        }                                                       \
        else if (iid == IID_##Interface1) {                     \
            return (Interface1*)this;                           \
        }                                                       \
        else if (iid == IID_##Interface2) {                     \
            return (Interface2*)this;                           \
        }                                                       \
        return SupperClassName::Probe(iid);                     \
    }                                                           \
                                                                \
    ECode ClassName::GetInterfaceID(                            \
        /* [in] */ IInterface* object,                          \
        /* [out] */ InterfaceID* iid)                           \
    {                                                           \
        VALIDATE_NOT_NULL(iid);                                 \
                                                                \
        if (object == (IInterface*)(Interface1*)this) {         \
            *iid = IID_##Interface1;                            \
        }                                                       \
        else if (object == (IInterface*)(Interface2*)this) {    \
            *iid = IID_##Interface2;                            \
        }                                                       \
        else {                                                  \
            return SupperClassName::GetInterfaceID(object, iid); \
        }                                                       \
        return NOERROR;                                         \
    }
#endif

#ifndef CCM_INTERFACE_IMPL_LIGHT_1
#define CCM_INTERFACE_IMPL_LIGHT_1(ClassName, InterfaceName) \
    Integer ClassName::AddRef(                             \
        /* [in] */ HANDLE id)                              \
    {                                                      \
        return LightRefBase::AddRef(id);                   \
    }                                                      \
                                                           \
    Integer ClassName::Release(                            \
        /* [in] */ HANDLE id)                              \
    {                                                      \
        return LightRefBase::Release(id);                  \
    }                                                      \
                                                           \
    IInterface* ClassName::Probe(                          \
        /* [in] */ const InterfaceID& iid)                 \
    {                                                      \
        if (iid == IID_IInterface) {                       \
            return (IInterface*)(InterfaceName*)this;      \
        }                                                  \
        else if (iid == IID_##InterfaceName) {             \
            return (InterfaceName*)this;                   \
        }                                                  \
        return nullptr;                                    \
    }                                                      \
                                                           \
    ECode ClassName::GetInterfaceID(                       \
        /* [in] */ IInterface* object,                     \
        /* [out] */ InterfaceID* iid)                      \
    {                                                      \
        VALIDATE_NOT_NULL(iid);                            \
                                                           \
        if (object == (IInterface*)(InterfaceName*)this) { \
            *iid = IID_##InterfaceName;                    \
            return NOERROR;                                \
        }                                                  \
        return E_ILLEGAL_ARGUMENT_EXCEPTION;               \
    }
#endif

#ifndef CCM_INTERFACE_IMPL_LIGHT_2
#define CCM_INTERFACE_IMPL_LIGHT_2(ClassName, InterfaceName1, InterfaceName2) \
    Integer ClassName::AddRef(                              \
        /* [in] */ HANDLE id)                               \
    {                                                       \
        return LightRefBase::AddRef(id);                    \
    }                                                       \
                                                            \
    Integer ClassName::Release(                             \
        /* [in] */ HANDLE id)                               \
    {                                                       \
        return LightRefBase::Release(id);                   \
    }                                                       \
                                                            \
    IInterface* ClassName::Probe(                           \
        /* [in] */ const InterfaceID& iid)                  \
    {                                                       \
        if (iid == IID_IInterface) {                        \
            return (IInterface*)(InterfaceName1*)this;      \
        }                                                   \
        else if (iid == IID_##InterfaceName1) {             \
            return (InterfaceName1*)this;                   \
        }                                                   \
        else if (iid == IID_##InterfaceName2) {             \
            return (InterfaceName2*)this;                   \
        }                                                   \
        return nullptr;                                     \
    }                                                       \
                                                            \
    ECode ClassName::GetInterfaceID(                        \
        /* [in] */ IInterface* object,                      \
        /* [out] */ InterfaceID* iid)                       \
    {                                                       \
        VALIDATE_NOT_NULL(iid);                             \
                                                            \
        if (object == (IInterface*)(InterfaceName1*)this) { \
            *iid = IID_##InterfaceName1;                    \
            return NOERROR;                                 \
        }                                                   \
        else if (object == (IInterface*)(InterfaceName2*)this) {    \
            *iid = IID_##InterfaceName2;                    \
            return NOERROR;                                 \
        }                                                   \
        return E_ILLEGAL_ARGUMENT_EXCEPTION;                \
    }
#endif

#ifndef CCM_OBJECT_DECL
#define CCM_OBJECT_DECL()                                  \
    ECode GetCoclassID(                                    \
        /* [out] */ CoclassID* cid) override;
#endif

#ifndef CCM_OBJECT_IMPL
#define CCM_OBJECT_IMPL(ClassName)                         \
    ECode ClassName::GetCoclassID(                         \
        /* [out] */ CoclassID* cid)                        \
    {                                                      \
        VALIDATE_NOT_NULL(cid);                            \
                                                           \
        *cid = CID_##ClassName;                            \
        return NOERROR;                                    \
    }
#endif

} // namespace ccm

#endif // __CCM_CCMDEF_H__
