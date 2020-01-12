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

#ifndef __COMO_TEXT_STRINGARRAYWRAPPER_H__
#define __COMO_TEXT_STRINGARRAYWRAPPER_H__

#include <comoarray.h>

namespace como {
namespace text {

class StringArrayWrapper
    : public LightRefBase
    , public IInterface
{
public:
    StringArrayWrapper(
        /* [in] */ const Array<String>& values)
        : mValues(values)
    {}

    Integer AddRef(
        /* [in] */ HANDLE id)
    {
        return LightRefBase::AddRef(id);
    }

    Integer Release(
        /* [in] */ HANDLE id)
    {
        return LightRefBase::Release(id);
    }

    IInterface* Probe(
        /* [in] */ const InterfaceID& iid)
    {
        if (iid == IID_IInterface) {
            return (IInterface*)this;
        }
        return nullptr;
    }

    ECode GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID& iid)
    {
        if (object == (IInterface*)this) {
            iid = IID_IInterface;
            return NOERROR;
        }
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

public:
    Array<String> mValues;
};

}
}

#endif // __COMO_TEXT_STRINGARRAYWRAPPER_H__