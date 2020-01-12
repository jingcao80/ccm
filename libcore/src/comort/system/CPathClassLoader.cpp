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

#include "como/core/StringUtils.h"
#include "comort/system/CPathClassLoader.h"

using como::core::StringUtils;

namespace comort {
namespace system {

ECode CPathClassLoader::Constructor (
    /* [in] */ const String& classPath,
    /* [in] */ IClassLoader* parent)
{
    mClassPath = StringUtils::Split(classPath, String(":"));
    FAIL_RETURN(ClassLoader::Constructor(parent));
    LoadComponentsInClassPath();
    return NOERROR;
}

ECode CPathClassLoader::FindCoclass(
    /* [in] */ const String& fullName,
    /* [out] */ AutoPtr<IMetaCoclass>& klass)
{
    for (Integer i = 0; i < mComponents.GetLength(); i++) {
        if (mComponents[i] == nullptr) {
            continue;
        }
        mComponents[i]->GetCoclass(fullName, klass);
        if (klass != nullptr) {
            return NOERROR;
        }
    }
    return E_CLASS_NOT_FOUND_EXCEPTION;
}

ECode CPathClassLoader::FindInterface(
    /* [in] */ const String& fullName,
    /* [out] */ AutoPtr<IMetaInterface>& intf)
{
    for (Integer i = 0; i < mComponents.GetLength(); i++) {
        if (mComponents[i] == nullptr) {
            continue;
        }
        mComponents[i]->GetInterface(fullName, intf);
        if (intf != nullptr) {
            return NOERROR;
        }
    }
    return E_INTERFACE_NOT_FOUND_EXCEPTION;
}

void CPathClassLoader::LoadComponentsInClassPath()
{
    AutoPtr<IClassLoader> parent;
    GetParent(parent);
    mComponents = Array<IMetaComponent*>(mClassPath.GetLength());
    for (Integer i = 0; i < mClassPath.GetLength(); i++) {
        AutoPtr<IMetaComponent> component;
        String path = mClassPath[i];
        if (!path.IsEmpty()) {
            parent->LoadComponent(path, component);
        }
        mComponents.Set(i, component.Get());
    }
}

}
}
