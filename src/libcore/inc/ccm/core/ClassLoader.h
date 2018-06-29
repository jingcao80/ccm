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

#ifndef __CCM_CORE_CLASSLOADER_H__
#define __CCM_CORE_CLASSLOADER_H__

#include "ccm/core/SyncObject.h"
#include "ccm.util.IHashMap.h"
#include <ccmautoptr.h>

using ccm::util::IHashMap;

namespace ccm {
namespace core {

class COM_PUBLIC ClassLoader
    : public SyncObject
    , public IClassLoader
{
private:
    class SystemClassLoader
    {
    public:
        static AutoPtr<IClassLoader> sLoader;
    };

public:
    CCM_INTERFACE_DECL();

    ECode LoadCoclass(
        /* [in] */ const String& fullName,
        /* [out] */ IMetaCoclass** klass) override;

    ECode GetParent(
        /* [out] */ IClassLoader** parent) override;

    static AutoPtr<IClassLoader> GetSystemClassLoader();

    ECode LoadInterface(
        /* [in] */ const String& fullName,
        /* [out] */ IMetaInterface** intf) override;

    ECode LoadComponent(
        /* [in] */ const String& path,
        /* [out] */ IMetaComponent** component) override;

    ECode LoadComponent(
        /* [in] */ const ComponentID& compId,
        /* [out] */ IMetaComponent** component) override;

    ECode UnloadComponent(
        /* [in] */ const ComponentID& compId) override;

protected:
    ECode Constructor(
        /* [in] */ IClassLoader* parent);

    virtual ECode FindCoclass(
        /* [in] */ const String& fullName,
        /* [out] */ IMetaCoclass** klass);

    virtual AutoPtr<IMetaCoclass> FindLoadedCoclass(
        /* [in] */ const String& fullName);

    virtual ECode FindInterface(
        /* [in] */ const String& fullName,
        /* [out] */ IMetaInterface** intf);

    virtual AutoPtr<IMetaInterface> FindLoadedInterface(
        /* [in] */ const String& fullName);

private:
    static AutoPtr<IClassLoader> CreateSystemClassLoader();

private:
    AutoPtr<IClassLoader> mParent;

    AutoPtr<IHashMap> mLoadedCoclasses;

    AutoPtr<IHashMap> mLoadedInterfaces;
};

inline AutoPtr<IClassLoader> ClassLoader::GetSystemClassLoader()
{
    return SystemClassLoader::sLoader;
}

}
}

#endif // __CCM_CORE_CLASSLOADER_H__
