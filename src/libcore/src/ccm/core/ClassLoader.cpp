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

#include "ccm/core/ClassLoader.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/core/System.h"
#include "ccm/util/CHashMap.h"
#include "ccmrt/system/CPathClassLoader.h"
#include <ccmapi.h>

using ccm::util::CHashMap;
using ccm::util::IID_IHashMap;
using ccmrt::system::CPathClassLoader;

namespace ccm {
namespace core {

CCM_INTERFACE_IMPL_1(ClassLoader, SyncObject, IClassLoader);

ECode ClassLoader::Constructor(
    /* [in] */ IClassLoader* parent)
{
    mParent = parent;
    CHashMap::New(IID_IHashMap, (IInterface**)&mLoadedCoclasses);
    CHashMap::New(IID_IHashMap, (IInterface**)&mLoadedInterfaces);
    return NOERROR;
}

ECode ClassLoader::LoadCoclass(
    /* [in] */ const String& fullName,
    /* [out] */ IMetaCoclass** klass)
{
    VALIDATE_NOT_NULL(klass);

    AutoPtr<IMetaCoclass> c = FindLoadedCoclass(fullName);
    if (c == nullptr) {
        if (mParent != nullptr) {
            mParent->LoadCoclass(fullName, (IMetaCoclass**)&c);
        }
        if (c == nullptr) {
            FAIL_RETURN(FindCoclass(fullName, (IMetaCoclass**)&c));
        }
        mLoadedCoclasses->Put(CoreUtils::Box(fullName), c);
    }
    *klass = c;
    REFCOUNT_ADD(*klass);
    return NOERROR;
}

AutoPtr<IClassLoader> ClassLoader::CreateSystemClassLoader()
{
    String classPath;
    System::GetProperty(String("ccm.class.path"), String("."), &classPath);

    AutoPtr<IClassLoader> cl;
    CPathClassLoader::New(classPath, CoGetBootClassLoader(),
            IID_IClassLoader, (IInterface**)&cl);
    CHECK(cl != nullptr);
    return cl;
}

ECode ClassLoader::FindCoclass(
    /* [in] */ const String& fullName,
    /* [out] */ IMetaCoclass** klass)
{
    return E_CLASS_NOT_FOUND_EXCEPTION;
}

AutoPtr<IMetaCoclass> ClassLoader::FindLoadedCoclass(
    /* [in] */ const String& fullName)
{
    AutoPtr<IInterface> value;
    mLoadedCoclasses->Get(CoreUtils::Box(fullName), (IInterface**)&value);
    return IMetaCoclass::Probe(value);
}

ECode ClassLoader::GetParent(
    /* [out] */ IClassLoader** parent)
{
    VALIDATE_NOT_NULL(parent);

    *parent = mParent;
    REFCOUNT_ADD(*parent);
    return NOERROR;
}

ECode ClassLoader::LoadInterface(
    /* [in] */ const String& fullName,
    /* [out] */ IMetaInterface** intf)
{
    VALIDATE_NOT_NULL(intf);

    AutoPtr<IMetaInterface> i = FindLoadedInterface(fullName);
    if (i == nullptr) {
        if (mParent != nullptr) {
            mParent->LoadInterface(fullName, (IMetaInterface**)&i);
        }
        if (i == nullptr) {
            FAIL_RETURN(FindInterface(fullName, (IMetaInterface**)&i));
            mLoadedInterfaces->Put(CoreUtils::Box(fullName), i);
        }
    }
    *intf = i;
    REFCOUNT_ADD(*intf);
    return NOERROR;
}

ECode ClassLoader::FindInterface(
    /* [in] */ const String& fullName,
    /* [out] */ IMetaInterface** intf)
{
    return E_INTERFACE_NOT_FOUND_EXCEPTION;
}

AutoPtr<IMetaInterface> ClassLoader::FindLoadedInterface(
    /* [in] */ const String& fullName)
{
    AutoPtr<IInterface> value;
    mLoadedInterfaces->Get(CoreUtils::Box(fullName), (IInterface**)&value);
    return IMetaInterface::Probe(value);
}

ECode ClassLoader::LoadComponent(
    /* [in] */ const String& path,
    /* [out] */ IMetaComponent** component)
{
    VALIDATE_NOT_NULL(component);

    if (mParent != nullptr) {
        return mParent->LoadComponent(path, component);
    }
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ClassLoader::LoadComponent(
    /* [in] */ const ComponentID& compId,
    /* [out] */ IMetaComponent** component)
{
    VALIDATE_NOT_NULL(component);

    if (mParent != nullptr) {
        return mParent->LoadComponent(compId, component);
    }
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ClassLoader::UnloadComponent(
    /* [in] */ const ComponentID& compId)
{
    if (mParent != nullptr) {
        return mParent->UnloadComponent(compId);
    }
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

AutoPtr<IInputStream> ClassLoader::GetSystemResourceAsStream(
    /* [in] */ const String& name)
{
    return nullptr;
}

//-------------------------------------------------------------------------

AutoPtr<IClassLoader> ClassLoader::SystemClassLoader::GetInstance()
{
    static AutoPtr<IClassLoader> sLoader = CreateSystemClassLoader();
    return sLoader;
}

}
}
