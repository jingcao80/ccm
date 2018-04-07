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

#include "ccmtypes.h"

namespace ccm {

void AssignFunc::operator()(
    /* [in] */ IInterface*& lvalue,
    /* [in] */ IInterface* rvalue,
    /* [in] */ void* id)
{
    if (rvalue != nullptr) {
        rvalue->AddRef(reinterpret_cast<HANDLE>(id));
    }
    if (lvalue != nullptr) {
        lvalue->Release(reinterpret_cast<HANDLE>(id));
    }
    lvalue = rvalue;
}

void DeleteFunc::operator()(
    /* [in] */ String* data,
    /* [in] */ Long size,
    /* [in] */ void* id)
{
    for (Long i = 0; i < size; i++) {
        data[i] = nullptr;
    }
    SharedBuffer::GetBufferFromData(data)->Release();
}

void DeleteFunc::operator()(
    /* [in] */ IInterface** data,
    /* [in] */ Long size,
    /* [in] */ void* id)
{
    for (Long i = 0; i < size; i++) {
        data[i]->Release(reinterpret_cast<HANDLE>(id));
        data[i] = nullptr;
    }
    SharedBuffer::GetBufferFromData(data)->Release();
}

}
