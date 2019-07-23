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

#include "ccm/text/CAttributedCharacterIteratorAttribute.h"
#include "ccm/text/CAttributedCharacterIteratorAttributeFactory.h"

namespace ccm {
namespace text {

CCM_INTERFACE_IMPL_1(CAttributedCharacterIteratorAttributeFactory, Object, IAttributedCharacterIteratorAttributeFactory);

CCM_OBJECT_IMPL(CAttributedCharacterIteratorAttributeFactory);

ECode CAttributedCharacterIteratorAttributeFactory::GetLANGUAGE(
    /* [out] */ IAttributedCharacterIteratorAttribute** attribute)
{
    return CAttributedCharacterIteratorAttribute::GetLANGUAGE(attribute);
}

ECode CAttributedCharacterIteratorAttributeFactory::GetREADING(
    /* [out] */ IAttributedCharacterIteratorAttribute** attribute)
{
    return CAttributedCharacterIteratorAttribute::GetREADING(attribute);
}

ECode CAttributedCharacterIteratorAttributeFactory::GetINPUT_METHOD_SEGMENT(
    /* [out] */ IAttributedCharacterIteratorAttribute** attribute)
{
    return CAttributedCharacterIteratorAttribute::GetINPUT_METHOD_SEGMENT(attribute);
}

}
}
