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

#ifndef __CDLC_CLASSOBJECTINTERFACEBUILDER_H__
#define __CDLC_CLASSOBJECTINTERFACEBUILDER_H__

#include "ast/CoclassType.h"
#include "ast/Module.h"
#include "parser/Phase.h"

namespace cdlc {

class ClassObjectInterfaceBuilder
    : public Phase
{
public:
    inline ClassObjectInterfaceBuilder(
        /* [in] */ World& world);

    bool Process() override;

private:
    void BuildCoclassObjectInterface(
        /* [in] */ CoclassType* klass);

private:
    static const char* TAG;

    Module* mModule = nullptr;
    AutoPtr<Type> mIInterfaceType;
    AutoPtr<Type> mInterfaceIDType;
    AutoPtr<Type> mIClassObjectType;
};

ClassObjectInterfaceBuilder::ClassObjectInterfaceBuilder(
    /* [in] */ World& world)
    : Phase(world)
    , mModule(world.GetWorkingModule())
{}

}

#endif // __CDLC_CLASSOBJECTINTERFACEBUILDER_H__
