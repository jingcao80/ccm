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

#include "parser/World.h"

namespace cdlc {

World* World::INSTANCE = nullptr;

AutoPtr<World> World::GetInstance()
{
    if (INSTANCE == nullptr) {
        INSTANCE = new World();
    }
    return INSTANCE;
}

AutoPtr<Module> World::GetWorkingModule()
{
    if (mWorkingModule == nullptr) {
        mWorkingModule = new Module();
    }
    return mWorkingModule;
}

AutoPtr<Module> World::FindModule(
    /* [in] */ const String& moduleName)
{
    if (moduleName.IsEmpty()) {
        return nullptr;
    }

    if (mCompilerRTModule->GetName().Equals(moduleName)) {
        return mCompilerRTModule;
    }

    for (AutoPtr<Module> module : mDependentModules) {
        if (module->GetName().Equals(moduleName)) {
            return module;
        }
    }

    return nullptr;
}

AutoPtr<EnumerationType> World::FindEnumeration(
    /* [in] */ const String& fullName)
{
    AutoPtr<Type> type = FindType(fullName);
    if (type != nullptr && type->IsEnumerationType()) {
        return (EnumerationType*)type.Get();
    }
    return nullptr;
}

AutoPtr<Type> World::FindType(
    /* [in] */ const String& name)
{
    AutoPtr<Type> type = mCompilerRTModule->FindType(name);
    if (type != nullptr) {
        return type;
    }
    for (AutoPtr<Module> module : mDependentModules) {
        type = module->FindType(name);
        if (type != nullptr) {
            return type;
        }
    }
    return nullptr;
}

}
