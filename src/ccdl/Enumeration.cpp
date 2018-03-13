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

#include "Enumeration.h"

#include <stdlib.h>

namespace ccm {
namespace ccdl {

Enumeration::~Enumeration()
{
    mNamespace = nullptr;
    for (int i = 0; i < mEnumIndex; i++) {
        Enumerator* e = mEnumerators[i];
        delete e;
    }
    if (mEnumerators != nullptr) free(mEnumerators);
}

Enumeration& Enumeration::AddEnumerator(
    /* [in] */ const String& name,
    /* [in] */ int value)
{
    if (name.IsNullOrEmpty()) return *this;

    if (mEnumIndex >= mEnumCapacity) {
        if (!EnlargeEnumeratorArray()) return *this;
    }

    Enumerator* e = new Enumerator(name, value);
    mEnumerators[mEnumIndex++] = e;
    return *this;
}

bool Enumeration::EnlargeEnumeratorArray()
{
    int newSize = mEnumCapacity == 0? 10 : mEnumCapacity + 10;
    Enumerator** newArray = (Enumerator**)calloc(sizeof(Enumerator*), newSize);
    if (newArray == nullptr) {
        return false;
    }

    memcpy(newArray, mEnumerators, sizeof(Enumerator*) * mEnumCapacity);
    mEnumCapacity = newSize;
    free(mEnumerators);
    mEnumerators = newArray;
    return true;
}

}
}