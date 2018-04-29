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

}
