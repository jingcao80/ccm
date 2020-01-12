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

#include "como/text/ParsePosition.h"

namespace como {
namespace text {

COMO_INTERFACE_IMPL_1(ParsePosition, SyncObject, IParsePosition);

ECode ParsePosition::Constructor(
    /* [in] */ Integer index)
{
    mIndex = index;
    return NOERROR;
}

ECode ParsePosition::GetIndex(
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    *index = mIndex;
    return NOERROR;
}

ECode ParsePosition::SetIndex(
    /* [in] */ Integer index)
{
    mIndex = index;
    return NOERROR;
}

ECode ParsePosition::SetErrorIndex(
    /* [in] */ Integer index)
{
    mErrorIndex = index;
    return NOERROR;
}

ECode ParsePosition::GetErrorIndex(
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    *index = mErrorIndex;
    return NOERROR;
}

ECode ParsePosition::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& same)
{
    if (IParsePosition::Probe(obj) == nullptr) {
        same = false;
        return NOERROR;
    }

    ParsePosition* other = (ParsePosition*)IParsePosition::Probe(obj);
    same = mIndex == other->mIndex &&
            mErrorIndex == other->mErrorIndex;
    return NOERROR;
}

ECode ParsePosition::GetHashCode(
    /* [out] */ Integer& hash)
{
    hash = (mErrorIndex << 16) | mIndex;
    return NOERROR;
}

ECode ParsePosition::ToString(
    /* [out] */ String& desc)
{
    desc = String::Format("%s[mIndex=%d, mErrorIndex=%d]",
            Object::GetCoclassName((IParsePosition*)this).string(),
            mIndex, mErrorIndex);
    return NOERROR;
}

}
}
