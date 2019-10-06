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

#ifndef __COMO_CSYSTEMCLASSLOADER_H__
#define __COMO_CSYSTEMCLASSLOADER_H__

#include "arraylist.h"
#include "comoptr.h"
#include "comoobj.h"
#include "util/hashmap.h"
#include "util/mutex.h"

namespace como {

extern const CoclassID CID_CSystemClassLoader;

COCLASS_ID(861efebf-54c8-4939-a2ab-4bacf2cafa1e)
class CBootClassLoader
    : public Object
    , public IClassLoader
{
public:
    static AutoPtr<IClassLoader> GetInstance();

    COMO_OBJECT_DECL();

    COMO_INTERFACE_DECL();

    ECode LoadComponent(
        /* [in] */ const String& path,
        /* [out] */ AutoPtr<IMetaComponent>& component) override;

    ECode LoadComponent(
        /* [in] */ const ComponentID& compId,
        /* [out] */ AutoPtr<IMetaComponent>& component) override;

    ECode UnloadComponent(
        /* [in] */ const ComponentID& compId) override;

    ECode LoadCoclass(
        /* [in] */ const String& fullName,
        /* [out] */ AutoPtr<IMetaCoclass>& klass) override;

    ECode LoadInterface(
        /* [in] */ const String& fullName,
        /* [out] */ AutoPtr<IMetaInterface>& intf) override;

    ECode GetParent(
        /* [out] */ AutoPtr<IClassLoader>& parent) override;

private:
    CBootClassLoader();

    void InitComponentPath();

    ECode FindComponent(
        /* [in] */ const ComponentID& compId,
        /* [out] */ String& compPath);

private:
    static const String TAG;
    static AutoPtr<IClassLoader> sInstance;

    Boolean mDebug;
    ArrayList<String> mComponentPath;
    HashMap<UUID, IMetaComponent*> mComponents;
    HashMap<String, IMetaComponent*> mComponentPathMap;
    Mutex mComponentsLock;
};

} // namespace como

#endif // __COMO_CSYSTEMCLASSLOADER_H__
