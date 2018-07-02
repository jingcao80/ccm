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

#include "ccmobject.h"
#include "ccmautoptr.h"

namespace ccm {

Integer Object::AddRef(
    /* [in] */ HANDLE id)
{
    return RefBase::AddRef(id);
}

Integer Object::Release(
    /* [in] */ HANDLE id)
{
    return RefBase::Release(id);
}

IInterface* Object::Probe(
    /* [in] */ const InterfaceID& iid)
{
    if (iid == IID_IInterface) {
        return (IInterface*)(IObject*)this;
    }
    else if (iid == IID_IObject) {
        return (IObject*)this;
    }
    else if (iid == IID_IWeakReferenceSource) {
        return (IWeakReferenceSource*)this;
    }
    return nullptr;
}

ECode Object::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);

    if (object == (IInterface*)(IObject*)this) {
        *iid = IID_IObject;
    }
    else if (object == (IWeakReferenceSource*)this) {
        *iid = IID_IWeakReferenceSource;
    }
    else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode Object::AttachMetadata(
    /* [in] */ IMetaComponent* component,
    /* [in] */ const String& coclassName)
{
    mComponent = component;
    mCoclassName = coclassName;
    return NOERROR;
}

ECode Object::GetCoclassID(
    /* [out] */ CoclassID* cid)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Object::GetCoclass(
    /* [out] */ IMetaCoclass** klass)
{
    VALIDATE_NOT_NULL(klass);

    if (mComponent != nullptr) {
        return mComponent->GetCoclass(mCoclassName, klass);
    }
    else {
        *klass = nullptr;
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
}

ECode Object::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    *hash = (Integer)reinterpret_cast<HANDLE>(this);
    return NOERROR;
}

ECode Object::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    *same = IObject::Probe(obj) == (IObject*)this;
    return NOERROR;
}

ECode Object::ToString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    AutoPtr<IMetaCoclass> mc;
    GetCoclass((IMetaCoclass**)&mc);
    String ns, name;
    if (mc != nullptr) {
        mc->GetNamespace(&ns);
        mc->GetName(&name);
    }
    *desc = String::Format("Object[0x%x], Class[%s%s]",
            this, ns.string(), name.string());
    return NOERROR;
}

ECode Object::GetWeakReference(
    /* [out] */ IWeakReference** wr)
{
    VALIDATE_NOT_NULL(wr);

    *wr = new WeakReferenceImpl((IObject*)this, CreateWeak(this));
    REFCOUNT_ADD(*wr)
    return NOERROR;
}

String Object::GetCoclassName(
    /* [in] */ IInterface* obj)
{
    String name;
    IObject* o = IObject::Probe(obj);
    if (o != nullptr) {
        AutoPtr<IMetaCoclass> mc;
        o->GetCoclass((IMetaCoclass**)&mc);
        if (mc != nullptr) {
            mc->GetName(&name);
        }
    }
    return name;
}

Integer Object::GetHashCode(
    /* [in] */ IInterface* obj)
{
    IObject* o = IObject::Probe(obj);
    if (o != nullptr) {
        Integer hash;
        o->GetHashCode(&hash);
        return hash;
    }
    return reinterpret_cast<uintptr_t>(IInterface::Probe(obj));
}

Boolean Object::Equals(
    /* [in] */ IInterface* obj1,
    /* [in] */ IInterface* obj2)
{
    if (IInterface::Probe(obj1) == IInterface::Probe(obj2)) {
        return true;
    }

    IObject* o1 = IObject::Probe(obj1);
    if (o1 == nullptr) {
        return false;
    }
    Boolean result;
    o1->Equals(obj2, &result);
    return result;
}

String Object::ToString(
    /* [in] */ IInterface* obj)
{
    if (obj == nullptr) {
        return String("null");
    }
    else {
        IObject* o = IObject::Probe(obj);
        if (o != nullptr) {
            String info;
            o->ToString(&info);
            return info;
        }
        else {
            return String("not a coclass object.");
        }
    }
}

}
