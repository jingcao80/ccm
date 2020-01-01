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

#include "como/security/CPermissions.h"
#include "como/security/CSecureRandom.h"
#include "como/security/action/CGetPropertyAction.h"
#include <ccmapi.h>
#include <new>

namespace como {
namespace security {

COMO_OBJECT_IMPL(CPermissions);
COMO_OBJECT_IMPL(CSecureRandom);
ECode CSecureRandom::New(
    /* [in] */ ISecureRandomSpi* secureRandomSpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& algorithm,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** object)
{
    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CSecureRandom, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CSecureRandom), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CSecureRandom* _obj = new(addr) CSecureRandom();
    ec = _obj->Constructor(secureRandomSpi, provider, algorithm);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }

    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    _obj->AttachMetadata(comp, String("como::security::CSecureRandom"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
};

namespace action {

COMO_OBJECT_IMPL(CGetPropertyAction);

}

}
}
