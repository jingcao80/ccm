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

#include "como/core/ClassLoader.h"
#include "como/core/CoreUtils.h"
#include "como/core/System.h"
#include "como/util/CHashMap.h"
#include "comort/system/CPathClassLoader.h"
#include <comoapi.h>

using como::util::CHashMap;
using como::util::IID_IHashMap;
using comort::system::CPathClassLoader;

namespace como {
namespace core {

COMO_INTERFACE_IMPL_1(ClassLoader, SyncObject, IClassLoader);

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
    /* [out] */ AutoPtr<IMetaCoclass>& klass)
{
    klass = FindLoadedCoclass(fullName);
    if (klass == nullptr) {
        if (mParent != nullptr) {
            mParent->LoadCoclass(fullName, klass);
        }
        if (klass == nullptr) {
            FAIL_RETURN(FindCoclass(fullName, klass));
        }
        mLoadedCoclasses->Put(CoreUtils::Box(fullName), klass);
    }
    return NOERROR;
}

AutoPtr<IClassLoader> ClassLoader::CreateSystemClassLoader()
{
    String classPath;
    System::GetProperty("como.class.path", ".", classPath);

    AutoPtr<IClassLoader> bcl, cl;
    CoGetBootClassLoader(bcl);
    CPathClassLoader::New(classPath, bcl, IID_IClassLoader, (IInterface**)&cl);
    CHECK(cl != nullptr);
    return cl;
}

ECode ClassLoader::FindCoclass(
    /* [in] */ const String& fullName,
    /* [out] */ AutoPtr<IMetaCoclass>& klass)
{
    return E_CLASS_NOT_FOUND_EXCEPTION;
}

AutoPtr<IMetaCoclass> ClassLoader::FindLoadedCoclass(
    /* [in] */ const String& fullName)
{
    AutoPtr<IInterface> value;
    mLoadedCoclasses->Get(CoreUtils::Box(fullName), value);
    return IMetaCoclass::Probe(value);
}

ECode ClassLoader::GetParent(
    /* [out] */ AutoPtr<IClassLoader>& parent)
{
    parent = mParent;
    return NOERROR;
}

ECode ClassLoader::LoadInterface(
    /* [in] */ const String& fullName,
    /* [out] */ AutoPtr<IMetaInterface>& intf)
{
    intf = FindLoadedInterface(fullName);
    if (intf == nullptr) {
        if (mParent != nullptr) {
            mParent->LoadInterface(fullName, intf);
        }
        if (intf == nullptr) {
            FAIL_RETURN(FindInterface(fullName, intf));
            mLoadedInterfaces->Put(CoreUtils::Box(fullName), intf);
        }
    }
    return NOERROR;
}

ECode ClassLoader::FindInterface(
    /* [in] */ const String& fullName,
    /* [out] */ AutoPtr<IMetaInterface>& intf)
{
    return E_INTERFACE_NOT_FOUND_EXCEPTION;
}

AutoPtr<IMetaInterface> ClassLoader::FindLoadedInterface(
    /* [in] */ const String& fullName)
{
    AutoPtr<IInterface> value;
    mLoadedInterfaces->Get(CoreUtils::Box(fullName), value);
    return IMetaInterface::Probe(value);
}

ECode ClassLoader::LoadComponent(
    /* [in] */ const String& path,
    /* [out] */ AutoPtr<IMetaComponent>& component)
{
    if (mParent != nullptr) {
        return mParent->LoadComponent(path, component);
    }
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ClassLoader::LoadComponent(
    /* [in] */ const ComponentID& compId,
    /* [out] */ AutoPtr<IMetaComponent>& component)
{
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
