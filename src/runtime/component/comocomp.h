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

#ifndef __COMO_COMPONENT_H__
#define __COMO_COMPONENT_H__

#include "comotypes.h"

namespace como {

typedef ECode (*GetterPtr)(AutoPtr<IClassObject>&);

struct ClassObjectGetter
{
    CoclassID   mCid;
    GetterPtr   mGetter;
};

typedef ECode (*GetClassObjectPtr)(const CoclassID&, AutoPtr<IClassObject>&);
typedef ClassObjectGetter* (*GetAllClassObjectsPtr)(int& size);
typedef Boolean (*CanUnloadPtr)();

struct MetadataWrapper
{
    int             mSize;
    unsigned char   mMetadata[0];
};

struct ComoComponent
{
    void*                   mSoHandle;
    GetClassObjectPtr       mSoGetClassObject;
    GetAllClassObjectsPtr   mSoGetAllClassObjects;
    CanUnloadPtr            mSoCanUnload;
    MetadataWrapper*        mMetadataWrapper;
};

} // namespace como

#endif // __COMO_COMPONENT_H__
