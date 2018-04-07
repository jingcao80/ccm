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

#include "ccmcomponent.h"
#include "ccmobjectapi.h"
#include "util/ccmautoptr.h"

namespace ccm {

ECode CoCreateObjectInstance(
    /* [in] */ const CoclassID& cid,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** object)
{
    AutoPtr<IClassObject> factory;
    ECode ec = CoAcquireClassFactory(cid, (IInterface**)&factory);
    if (FAILED(ec)) {
        *object = nullptr;
        return ec;
    }

    return factory->CreateObject(iid, object);
}

ECode CoAcquireClassFactory(
    /* [in] */ const CoclassID& cid,
    /* [out] */ IInterface** object)
{
    CcmComponent* ccmComp;
    ECode ec = CoGetComponent(*cid.mCid, &ccmComp);
    if (FAILED(ec)) {
        *object = nullptr;
        return ec;
    }

    return ccmComp->mSoGetClassObject(cid, object);
}

}
