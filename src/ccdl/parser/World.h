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

#ifndef __CCDL_WORLD_H__
#define __CCDL_WORLD_H__

#include "../ast/BooleanType.h"
#include "../ast/ByteType.h"
#include "../ast/CharType.h"
#include "../ast/CoclassIDType.h"
#include "../ast/ComponentIDType.h"
#include "../ast/DoubleType.h"
#include "../ast/Enumeration.h"
#include "../ast/FloatType.h"
#include "../ast/HANDLEType.h"
#include "../ast/IntegerType.h"
#include "../ast/Interface.h"
#include "../ast/InterfaceIDType.h"
#include "../ast/LongType.h"
#include "../ast/Module.h"
#include "../ast/Namespace.h"
#include "../ast/Pool.h"
#include "../ast/ShortType.h"
#include "../ast/StringType.h"
#include "../ast/Type.h"
#include "../util/ArrayList.h"
#include "../util/StringMap.h"

#include <memory.h>

using ccdl::ast::BooleanType;
using ccdl::ast::ByteType;
using ccdl::ast::CharType;
using ccdl::ast::CoclassIDType;
using ccdl::ast::ComponentIDType;
using ccdl::ast::DoubleType;
using ccdl::ast::Enumeration;
using ccdl::ast::FloatType;
using ccdl::ast::HANDLEType;
using ccdl::ast::IntegerType;
using ccdl::ast::Interface;
using ccdl::ast::InterfaceIDType;
using ccdl::ast::LongType;
using ccdl::ast::Module;
using ccdl::ast::Namespace;
using ccdl::ast::Pool;
using ccdl::ast::ShortType;
using ccdl::ast::StringType;
using ccdl::ast::Type;

namespace ccdl {

class World : public Pool
{
public:
    World();

    ~World();

    inline void SetRootFile(
        /* [in] */ const String& rootFile);

    Namespace* GetGlobalNamespace();

    std::shared_ptr<Module> CreateWorkingModule();

    inline std::shared_ptr<Module> GetWorkingModule();

    String Dump(
        /* [in] */ const String& prefix) override;

private:
    String mRootFile;
    std::shared_ptr<Module> mWorkingModule;
    ArrayList<Module*> mExternalModules;

    ByteType* mByteType;
    ShortType* mShortType;
    IntegerType* mIntegerType;
    LongType* mLongType;
    CharType* mCharType;
    FloatType* mFloatType;
    DoubleType* mDoubleType;
    BooleanType* mBooleanType;
    StringType* mStringType;
    HANDLEType* mHANDLEType;
    CoclassIDType* mCoclassIDType;
    ComponentIDType* mComponentIDType;
    InterfaceIDType* mInterfaceIDType;
};

void World::SetRootFile(
    /* [in] */ const String& rootFile)
{
    mRootFile = rootFile;
}

std::shared_ptr<Module> World::GetWorkingModule()
{
    return mWorkingModule;
}

}

#endif // __CCDL_WORLD_H__
