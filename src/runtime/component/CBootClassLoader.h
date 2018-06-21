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

#ifndef __CCM_CSYSTEMCLASSLOADER_H__
#define __CCM_CSYSTEMCLASSLOADER_H__

#include "arraylist.h"
#include "ccmautoptr.h"
#include "ccmobject.h"
#include "util/hashmap.h"
#include "util/mutex.h"

namespace ccm {

extern const CoclassID CID_CSystemClassLoader;

COCLASS_ID(861efebf-54c8-4939-a2ab-4bacf2cafa1e)
class CBootClassLoader
    : public Object
    , public IClassLoader
{
public:
    static AutoPtr<IClassLoader> GetInstance();

    CCM_OBJECT_DECL();

    CCM_INTERFACE_DECL();

    ECode LoadComponent(
        /* [in] */ const ComponentID& compId,
        /* [out] */ IMetaComponent** component);

    ECode UnloadComponent(
        /* [in] */ const ComponentID& compId);

    ECode LoadCoclass(
        /* [in] */ const String& fullName,
        /* [out] */ IMetaCoclass** klass);

    ECode LoadInterface(
        /* [in] */ const String& fullName,
        /* [out] */ IMetaInterface** intf);

private:
    CBootClassLoader();

    void InitClassPath();

    ECode FindComponent(
        /* [in] */ const ComponentID& compId,
        /* [out] */ String* compPath);

private:
    static AutoPtr<IClassLoader> sInstance;
    static const String TAG;
    Boolean mDebug;
    ArrayList<String> mClassPath;
    HashMap<Uuid, IMetaComponent*> mComponents;
    Mutex mComponentsLock;
};

}

#endif // __CCM_CSYSTEMCLASSLOADER_H__
