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

#include "ccmrt/system/CPathClassLoader.h"

namespace ccmrt {
namespace system {

ECode CPathClassLoader::Constructor (
    /* [in] */ const String& classPath,
    /* [in] */ IClassLoader* parent)
{
    return NOERROR;
}

ECode CPathClassLoader::Constructor (
    /* [in] */ const String& classPath,
    /* [in] */ const String& librarySearchPath,
    /* [in] */ IClassLoader* parent)
{
    return NOERROR;
}

ECode CPathClassLoader::LoadComponent(
    /* [in] */ const ComponentID& compId,
    /* [out] */ IMetaComponent** component)
{
    return NOERROR;
}

ECode CPathClassLoader::UnloadComponent(
    /* [in] */ const ComponentID& compId)
{
    return NOERROR;
}

ECode CPathClassLoader::LoadCoclass(
    /* [in] */ const String& fullName,
    /* [out] */ IMetaCoclass** klass)
{
    return NOERROR;
}

ECode CPathClassLoader::LoadInterface(
    /* [in] */ const String& fullName,
    /* [out] */ IMetaInterface** intf)
{
    return NOERROR;
}

}
}
