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

#ifndef __CCM_TEXT_ATTRIBUTEDCHARACTERITERATORATTRIBUTE_H__
#define __CCM_TEXT_ATTRIBUTEDCHARACTERITERATORATTRIBUTE_H__

#include "ccm/core/SyncObject.h"
#include "ccm.text.IAttributedCharacterIteratorAttribute.h"
#include "ccm.util.IMap.h"

using ccm::core::SyncObject;
using ccm::util::IMap;

namespace ccm {
namespace text {

class AttributedCharacterIteratorAttribute
    : public SyncObject
    , public IAttributedCharacterIteratorAttribute
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ const String& name);

    ECode ToString(
        /* [out] */ String* str) override;

    static ECode GetLANGUAGE(
        /* [out] */ IAttributedCharacterIteratorAttribute** attribute);

    static ECode GetREADING(
        /* [out] */ IAttributedCharacterIteratorAttribute** attribute);

    static ECode GetINPUT_METHOD_SEGMENT(
        /* [out] */ IAttributedCharacterIteratorAttribute** attribute);

protected:
    virtual String GetName();

    virtual ECode ReadResolve(
        /* [out] */ IInterface** obj);

private:
    static AutoPtr<IMap> GetInstanceMap();

private:
    String mName;
};

}
}

#endif // __CCM_TEXT_ATTRIBUTEDCHARACTERITERATORATTRIBUTE_H__
