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

#ifndef __CCM_CCMTYPE_H__
#define __CCM_CCMTYPE_H__

#include "ccmdef.h"
#include "ccmuuid.h"
#include <stdlib.h>

namespace ccm {

typedef char            Byte;
typedef short           Short;
typedef int             Integer;
typedef long long int   Long;
typedef float           Float;
typedef double          Double;
typedef char32_t        Char;
typedef bool            Boolean;
typedef uintptr_t       HANDLE;
typedef int             ECode;

struct ComponentID;

struct CoclassID
{
    Uuid                mUuid;
    const ComponentID*  mCid;

    static const CoclassID Null;
};

inline bool operator==(
    /* [in] */ const CoclassID& cid1,
    /* [in] */ const CoclassID& cid2)
{
    return !memcmp(&cid1.mUuid, &cid2.mUuid, sizeof(Uuid));
}

inline bool operator!=(
    /* [in] */ const CoclassID& cid1,
    /* [in] */ const CoclassID& cid2)
{
    return memcmp(&cid1.mUuid, &cid2.mUuid, sizeof(Uuid));
}

struct InterfaceID
{
    Uuid                mUuid;
    const ComponentID*  mCid;

    static const InterfaceID Null;
};

inline bool operator==(
    /* [in] */ const InterfaceID& iid1,
    /* [in] */ const InterfaceID& iid2)
{
    return !memcmp(&iid1.mUuid, &iid2.mUuid, sizeof(Uuid));
}

inline bool operator!=(
    /* [in] */ const InterfaceID& iid1,
    /* [in] */ const InterfaceID& iid2)
{
    return memcmp(&iid1.mUuid, &iid2.mUuid, sizeof(Uuid));
}

struct ComponentID
{
    Uuid                mUuid;
    const char*         mUrl;
};

inline bool operator==(
    /* [in] */ const ComponentID& cid1,
    /* [in] */ const ComponentID& cid2)
{
    return !memcmp(&cid1.mUuid, &cid2.mUuid, sizeof(Uuid));
}

inline bool operator!=(
    /* [in] */ const ComponentID& cid1,
    /* [in] */ const ComponentID& cid2)
{
    return memcmp(&cid1.mUuid, &cid2.mUuid, sizeof(Uuid));
}

}

#include "ccmerror.h"
#include "ccmstring.h"

namespace ccm {

extern String DumpUuid(
    /* [in] */ const Uuid& id);

extern Integer HashUuid(
    /* [in] */ const Uuid& key);

extern const ComponentID CID_CCMRuntime;
extern const InterfaceID IID_IInterface;

INTERFACE_ID(00000000-0000-0000-0000-000000000001)
interface IInterface
{
    inline static IInterface* Probe(
        /* [in] */ IInterface* object)
    {
        if (object == nullptr) return nullptr;
        return object->Probe(IID_IInterface);
    }

    virtual Integer AddRef(
        /* [in] */ HANDLE id = 0) = 0;

    virtual Integer Release(
        /* [in] */ HANDLE id = 0) = 0;

    virtual IInterface* Probe(
        /* [in] */ const InterfaceID& iid) = 0;

    virtual ECode GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid) = 0;

    inline static Boolean Equals(
        /* [in] */ IInterface* object1,
        /* [in] */ IInterface* object2)
    {
        if (object1 == nullptr || object2 == nullptr) {
            return false;
        }
        return IInterface::Probe(object1) == IInterface::Probe(object2);
    }
};

}

#include "ccmintfs.h"
#include "ccmsharedbuffer.h"
#include "ccmtypekind.h"

namespace ccm {

struct COM_PUBLIC Triple
{
    Triple()
        : mData(nullptr)
        , mSize(0)
        , mType(CcmTypeKind::Unknown)
    {}

    Triple(
        /* [in] */ const Triple& other);

    void AllocData(
        /* [in] */ Long dataSize);

    void FreeData();

    Triple& operator=(
        /* [in] */ const Triple& other);

    Triple& operator=(
        /* [in] */ Triple&& other);

    void* mData;
    Long mSize;
    CcmTypeKind mType;
};

template<class T>
struct Type2Kind
{
    static CcmTypeKind Kind()
    {
        return CcmTypeKind::Unknown;
    }

    enum { isPrimitiveType = false };
    enum { isStringType = false };
};

#define TYPE2KIND_SPEC(type, kind, value)       \
        template<>                              \
        struct Type2Kind<type>                  \
        {                                       \
            inline static CcmTypeKind Kind()    \
            {                                   \
                return kind;                    \
            }                                   \
            enum { isPrimitiveType = value };   \
            enum { isStringType = (kind == CcmTypeKind::String) };  \
        };

TYPE2KIND_SPEC(Byte, CcmTypeKind::Byte, true);
TYPE2KIND_SPEC(Short, CcmTypeKind::Short, true);
TYPE2KIND_SPEC(Integer, CcmTypeKind::Integer, true);
TYPE2KIND_SPEC(Long, CcmTypeKind::Long, true);
TYPE2KIND_SPEC(Float, CcmTypeKind::Float, true);
TYPE2KIND_SPEC(Double, CcmTypeKind::Double, true);
TYPE2KIND_SPEC(Char, CcmTypeKind::Char, true);
TYPE2KIND_SPEC(Boolean, CcmTypeKind::Boolean, true);
TYPE2KIND_SPEC(String, CcmTypeKind::String, true);
TYPE2KIND_SPEC(HANDLE, CcmTypeKind::HANDLE, true);
TYPE2KIND_SPEC(CoclassID, CcmTypeKind::CoclassID, true);
TYPE2KIND_SPEC(ComponentID, CcmTypeKind::ComponentID, true);
TYPE2KIND_SPEC(InterfaceID, CcmTypeKind::InterfaceID, true);
TYPE2KIND_SPEC(IInterface*, CcmTypeKind::Interface, false);
TYPE2KIND_SPEC(Uuid, CcmTypeKind::Unknown, true);

template<class T, class U>
class Conversion
{
    typedef char Small;
    class Big { char dummy[2]; };
    static Small Test(U);
    static Big Test(...);
    static T MakeT();
public:
    enum { exists = sizeof(Test(MakeT())) == sizeof(Small) };
    enum { exists2Way = exists && Conversion<U, T>::exists };
    enum { sameType = false };
};

template<class T>
class Conversion<T, T>
{
public:
    enum { exists = 1, exists2Way = 1, sameType = 1 };
};

#define SUPERSUBCLASS(T, U)                         \
    (Conversion<const U*, const T*>::exists &&      \
     !Conversion<const T*, const void*>::sameType)

#define SUPERSUBCLASS_STRICT(T, U)                  \
    (SUPERSUBCLASS(T,U) &&                          \
     !Conversion<const T, const U>::sameType)

template<class T>
class TypeTraits
{
public:
    typedef T   BareType;
    enum { isPointer = 0 };
    enum { isArray = SUPERSUBCLASS_STRICT(Triple, T) };
};

template<class T>
class TypeTraits<T*>
{
public:
    typedef T   BareType;
    enum { isPointer = 1 };
    enum { isArray = 0 };
};

template<class T>
class TypeTraits<const T*>
{
public:
    typedef T   BareType;
    enum { isPointer = 1 };
    enum { isArray = 0 };
};

#define CREATE_MEMBER_DETECTOR(X)                                                   \
template<class T>                                                                   \
class Detect_##X                                                                    \
{                                                                                   \
    struct Fallback { int X; };                                                     \
    struct Derived : T, Fallback { };                                               \
                                                                                    \
    template<class U, U> struct Check;                                              \
                                                                                    \
    typedef char ArrayOfOne[1];                                                     \
    typedef char ArrayOfTwo[2];                                                     \
                                                                                    \
    template<class U> static ArrayOfOne & func(Check<int Fallback::*, &U::X> *);    \
    template<class U> static ArrayOfTwo & func(...);                                \
public:                                                                             \
    typedef Detect_##X type;                                                        \
    enum { exists = sizeof(func<Derived>(0)) == 2 };                                \
};

CREATE_MEMBER_DETECTOR(AddRef);
CREATE_MEMBER_DETECTOR(Release);

#define HAS_ADDREF_AND_RELEASE(type) \
    (Detect_AddRef<type>::exists && Detect_Release<type>::exists)

//-----------------------------------------------------------------

template<class T, Boolean = Type2Kind<T>::isPrimitiveType>
struct InitFunc
{
    void operator()(
        /* [in] */ T* data,
        /* [in] */ void* id)
    {
        *data = nullptr;
    }
};

template<class T>
struct InitFunc<T, true>
{
    void operator()(
        /* [in] */ T* data,
        /* [in] */ void* id)
    {
        *data = 0;
    }
};

template<>
struct InitFunc<Uuid, true>
{
    inline void operator()(
        /* [in] */ Uuid* data,
        /* [in] */ void* id)
    {
        memset(data, 0, sizeof(Uuid));
    }
};

//-----------------------------------------------------------------

template<class T, Boolean hasAddRefAndRelease>
struct AssignImpl
{
    void operator()(
        /* [in] */ T* lvalue,
        /* [in] */ const T& rvalue,
        /* [in] */ void* id)
    {
        *lvalue = rvalue;
    }
};

template<class T>
struct AssignImpl<T, true>
{
    void operator()(
        /* [in] */ T* lvalue,
        /* [in] */ const T& rvalue,
        /* [in] */ void* id)
    {
        if (rvalue != nullptr) {
            rvalue->AddRef(reinterpret_cast<HANDLE>(id));
        }
        if (*lvalue != nullptr) {
            (*lvalue)->Release(reinterpret_cast<HANDLE>(id));
        }
        *lvalue = rvalue;
    }
};

template<class T, Boolean = Type2Kind<T>::isPrimitiveType>
struct AssignFunc
{
    void operator()(
        /* [in] */ T* lvalue,
        /* [in] */ const T& rvalue,
        /* [in] */ void* id)
    {
        typedef typename TypeTraits<T>::BareType BareType;
        AssignImpl<T, HAS_ADDREF_AND_RELEASE(BareType)> impl;
        impl(lvalue, rvalue, id);
    }
};

template<class T>
struct AssignFunc<T, true>
{
    void operator()(
        /* [in] */ T* lvalue,
        /* [in] */ const T& rvalue,
        /* [in] */ void* id)
    {
        *lvalue = rvalue;
    }
};

//-----------------------------------------------------------------

template<class T, Boolean = TypeTraits<T>::isArray>
struct DeleteTriple
{
    void operator()(
        /* [in] */ T* data,
        /* [in] */ void* id)
    {}
};

template<class T>
struct DeleteTriple<Array<T>, true>
{
    void operator()(
        /* [in] */ Array<T>* data,
        /* [in] */ void* id)
    {
        if (data->mData != nullptr) {
            SharedBuffer* sb = SharedBuffer::GetBufferFromData(data->mData);
            if (sb->OnlyOwner()) {
                if (Type2Kind<T>::isStringType) {
                    String* payload = reinterpret_cast<String*>(data->mData);
                    for (Long i = 0; i < data->mSize; i++) {
                        payload[i] = nullptr;
                    }
                }
                if (!Type2Kind<T>::isPrimitiveType) {
                    IInterface** payload = reinterpret_cast<IInterface**>(data->mData);
                    for (Long i = 0; i < data->mSize; i++) {
                        payload[i]->Release();
                    }
                }
            }
            sb->Release();
        }
    }
};

template<class T, Boolean hasAddRefAndRelease>
struct DeleteImpl
{
    void operator()(
        /* [in] */ T* data,
        /* [in] */ void* id)
    {
        if (TypeTraits<T>::isArray) {
            DeleteTriple<T> func;
            func(data, id);
        }
    }
};

template<class T>
struct DeleteImpl<T, true>
{
    void operator()(
        /* [in] */ T* data,
        /* [in] */ void* id)
    {
        if (*data != nullptr) {
            (*data)->Release(reinterpret_cast<HANDLE>(id));
            *data = nullptr;
        }
    }
};

template<class T, Boolean = Type2Kind<T>::isPrimitiveType>
struct DeleteFunc
{
    void operator()(
        /* [in] */ T* data,
        /* [in] */ void* id)
    {
        typedef typename TypeTraits<T>::BareType BareType;
        DeleteImpl<T, HAS_ADDREF_AND_RELEASE(BareType)> impl;
        impl(data, id);
    }
};

template<class T>
struct DeleteFunc<T, true>
{
    void operator()(
        /* [in] */ T* data,
        /* [in] */ void* id)
    {}
};

template<>
struct DeleteFunc<String, true>
{
    inline void operator()(
        /* [in] */ String* data,
        /* [in] */ void* id)
    {
        *data = nullptr;
    }
};

//-----------------------------------------------------------------

template<class T>
struct CompareFunc
{
    Integer operator()(
        /* [in] */ const T& lvalue,
        /* [in] */ const T& rvalue)
    {
        if (lvalue > rvalue) return 1;
        if (lvalue == rvalue) return 0;
        return -1;
    }
};

template<>
struct CompareFunc<String>
{
    inline Integer operator()(
        /* [in] */ const String& lvalue,
        /* [in] */ const String& rvalue)
    {
        return lvalue.Compare(rvalue);
    }
};

template<>
struct CompareFunc<Uuid>
{
    inline Integer operator()(
        /* [in] */ const Uuid& lvalue,
        /* [in] */ const Uuid& rvalue)
    {
        return lvalue == rvalue ? 0 : -1;
    }
};

//-----------------------------------------------------------------

template<class T>
struct HashFunc
{
    Integer operator()(
        /* [in] */ const T& data)
    {
        return data;
    }
};

template<>
struct HashFunc<String>
{
    inline Integer operator()(
        /* [in] */ const String& data)
    {
        return data.GetHashCode();
    }
};

template<>
struct HashFunc<Uuid>
{
    inline Integer operator()(
        /* [in] */ const Uuid& data)
    {
        return HashUuid(data);
    }
};

}

#include "ccmarray.h"

#endif // __CCM_CCMTYPE_H__
