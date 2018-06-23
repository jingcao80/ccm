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
#include <ccmautoptr.h>

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
        /* [out] */ IMetaCoclass** klass);

    static AutoPtr<IClassLoader> GetSystemClassLoader();

protected:
    AutoPtr<IMetaCoclass> FindLoadedClass(
        /* [in] */ const String& name);

private:
    static AutoPtr<IClassLoader> CreateSystemClassLoader();

private:
    AutoPtr<IClassLoader> mParent;
};

inline AutoPtr<IClassLoader> ClassLoader::GetSystemClassLoader()
{
    return SystemClassLoader::sLoader;
}

}
}

#endif // __CCM_CORE_CLASSLOADER_H__
