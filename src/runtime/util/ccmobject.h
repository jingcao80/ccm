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

#ifndef __CCM_OBJECT_H__
#define __CCM_OBJECT_H__

#include "ccmrefbase.h"
#include "ccmtypes.h"

namespace ccm {

class COM_PUBLIC Object
    : public RefBase
    , public IObject
    , public IWeakReferenceSource
{
public:
    CCM_INTERFACE_DECL();

    ECode AttachMetadata(
        /* [in] */ IMetaComponent* component,
        /* [in] */ const String& coclassName) override;

    ECode GetCoclassID(
        /* [out] */ CoclassID* cid) override;

    ECode GetCoclass(
        /* [out] */ IMetaCoclass** klass) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* same) override;

    ECode ToString(
        /* [out] */ String* desc) override;

    ECode GetWeakReference(
        /* [out] */ IWeakReference** wr) override;

private:
    IMetaComponent* mComponent;
    String mCoclassName;
};

}

#endif // __CCM_OBJECT_H__
