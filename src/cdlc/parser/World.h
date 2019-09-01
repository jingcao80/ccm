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

#ifndef __CDLC_WORLD_H__
#define __CDLC_WORLD_H__

#include "util/AutoPtr.h"
#include "util/LightRefBase.h"
#include "util/String.h"
#include "ast/EnumerationType.h"
#include "ast/Module.h"
#include "ast/Type.h"

namespace cdlc {

class World
    : public LightRefBase
{
public:
    inline World();

    inline AutoPtr<Module> GetCompilerRTModule();

    AutoPtr<Module> GetWorkingModule();

    AutoPtr<EnumerationType> FindEnumeration(
        /* [in] */ const String& fullName);

    AutoPtr<Type> FindType(
        /* [in] */ const String& name);

private:
    AutoPtr<Module> mCompilerRTModule;
    AutoPtr<Module> mWorkingModule;
};

World::World()
{
    mCompilerRTModule = new Module();
}

AutoPtr<Module> World::GetCompilerRTModule()
{
    return mCompilerRTModule;
}

}

#endif // __CDLC_WORLD_H__
