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

#include "World.h"
#include "../util/StringBuilder.h"

namespace ccdl {

World::World()
    : mWorkingModule(nullptr)
    , mExternalModules(10)
{
    Namespace* global = new Namespace(String("__global__"));
    AddNamespace(global);
    Namespace* ccm = new Namespace(String("ccm"));
    AddNamespace(ccm);
    global->AddNamespace(ccm);

    mByteType = new ByteType();
    mByteType->SetNamespace(ccm);
    mShortType = new ShortType();
    mShortType->SetNamespace(ccm);
    mIntegerType = new IntegerType();
    mIntegerType->SetNamespace(ccm);
    mLongType = new LongType();
    mLongType->SetNamespace(ccm);
    mCharType = new CharType();
    mCharType->SetNamespace(ccm);
    mFloatType = new FloatType();
    mFloatType->SetNamespace(ccm);
    mDoubleType = new DoubleType();
    mDoubleType->SetNamespace(ccm);
    mBooleanType = new BooleanType();
    mBooleanType->SetNamespace(ccm);
    mStringType = new StringType();
    mStringType->SetNamespace(ccm);
    mHANDLEType = new HANDLEType();
    mHANDLEType->SetNamespace(ccm);
    mECodeType = new ECodeType();
    mECodeType->SetNamespace(ccm);
    mCoclassIDType = new CoclassIDType();
    mCoclassIDType->SetNamespace(ccm);
    mComponentIDType = new ComponentIDType();
    mComponentIDType->SetNamespace(ccm);
    mInterfaceIDType = new InterfaceIDType();
    mInterfaceIDType->SetNamespace(ccm);

    mTypes.Put(String("Byte"), mByteType);
    mTypes.Put(String("Short"), mShortType);
    mTypes.Put(String("Integer"), mIntegerType);
    mTypes.Put(String("Long"), mLongType);
    mTypes.Put(String("Char"), mCharType);
    mTypes.Put(String("Float"), mFloatType);
    mTypes.Put(String("Double"), mDoubleType);
    mTypes.Put(String("Boolean"), mBooleanType);
    mTypes.Put(String("String"), mStringType);
    mTypes.Put(String("HANDLE"), mHANDLEType);
    mTypes.Put(String("ECode"), mECodeType);
    mTypes.Put(String("CoclassID"), mCoclassIDType);
    mTypes.Put(String("ComponentID"), mComponentIDType);
    mTypes.Put(String("InterfaceID"), mInterfaceIDType);
}

World::~World()
{
    delete mByteType;
    delete mShortType;
    delete mIntegerType;
    delete mLongType;
    delete mCharType;
    delete mFloatType;
    delete mDoubleType;
    delete mBooleanType;
    delete mStringType;
    delete mHANDLEType;
    delete mECodeType;
    delete mCoclassIDType;
    delete mComponentIDType;
    delete mInterfaceIDType;
}

Namespace* World::GetGlobalNamespace()
{
    return mNamespaces.Get(0);
}

std::shared_ptr<Module> World::CreateWorkingModule()
{
    if (mWorkingModule == nullptr) {
        mWorkingModule = std::make_shared<Module>();
    }
    return mWorkingModule;
}

Type* World::ResolveType(
    /* [in] */ const String& fullName)
{
    return nullptr;
}

bool World::AddExternalModule(
    /* [in] */ Module* module)
{
    if (module == nullptr) {
        return true;
    }

    return mExternalModules.Add(module);
}

Type* World::FindTypeInExternalModules(
    /* [in] */ const String& fullName)
{
    if (fullName.IsNullOrEmpty()) {
        return nullptr;
    }

    for (int i = 0; i < mExternalModules.GetSize(); i++) {
        Type* type = mExternalModules.Get(i)->FindType(fullName);
        if (type != nullptr) {
            return type;
        }
    }
    return nullptr;
}

String World::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append("World[");
    builder.Append("root:").Append(mRootFile);
    builder.Append("]\n");
    builder.Append(Pool::Dump(prefix));

    if (mWorkingModule != nullptr) {
        builder.Append("\n");
        builder.Append(mWorkingModule->Dump(String("")));
    }
    return builder.ToString();
}

}
