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

#ifndef __CCMRT_SYSTEM_CPATHCLASSLOADER_H__
#define __CCMRT_SYSTEM_CPATHCLASSLOADER_H__

#include "ccm/core/ClassLoader.h"
#include "_ccmrt_system_CPathClassLoader.h"

using namespace ccm;
using ccm::core::ClassLoader;

namespace ccmrt {
namespace system {

Coclass(CPathClassLoader)
    , public ClassLoader
{
public:
    ECode Constructor(
        /* [in] */ const String& classPath,
        /* [in] */ IClassLoader* parent);

private:
    ECode FindCoclass(
        /* [in] */ const String& fullName,
        /* [out] */ IMetaCoclass** klass) override;

    ECode FindInterface(
        /* [in] */ const String& fullName,
        /* [out] */ IMetaInterface** intf) override;

    void LoadComponentsInClassPath();

private:
    Array<String> mClassPath;

    Array<IMetaComponent*> mComponents;
};

}
}

#endif // __CCMRT_SYSTEM_CPATHCLASSLOADER_H__
