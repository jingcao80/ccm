//=========================================================================
// Copyright (C) 2018 The C++ Module Model(CCM) Open Source Project
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

#include "Module.h"
#include "../metadata/MetaResolver.h"
#include "../util/StringBuilder.h"

using ccdl::metadata::MetaResolver;

namespace ccdl {
namespace ast {

Module::Module(
    /* [in] */ void* metadata)
    : mMetaComponent((MetaComponent*)metadata)
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

    mTypes.Put(String("ccm::Byte"), mByteType);
    mTypes.Put(String("ccm::Short"), mShortType);
    mTypes.Put(String("ccm::Integer"), mIntegerType);
    mTypes.Put(String("ccm::Long"), mLongType);
    mTypes.Put(String("ccm::Char"), mCharType);
    mTypes.Put(String("ccm::Float"), mFloatType);
    mTypes.Put(String("ccm::Double"), mDoubleType);
    mTypes.Put(String("ccm::Boolean"), mBooleanType);
    mTypes.Put(String("ccm::String"), mStringType);
    mTypes.Put(String("ccm::HANDLE"), mHANDLEType);
    mTypes.Put(String("ccm::ECode"), mECodeType);
    mTypes.Put(String("ccm::CoclassID"), mCoclassIDType);
    mTypes.Put(String("ccm::ComponentID"), mComponentIDType);
    mTypes.Put(String("ccm::InterfaceID"), mInterfaceIDType);
}

Module::~Module()
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

    if (mMetaComponent != nullptr) {
        free(mMetaComponent);
    }
}

Namespace* Module::GetGlobalNamespace()
{
    return mNamespaces.Get(0);
}

void Module::SetAttribute(
    /* [in] */ const Attribute& attr)
{
    mUuid.Parse(attr.mUuid);
    mVersion = attr.mVersion;
    mDescription = attr.mDescription;
    mUrl = attr.mUrl;
}

Type* Module::ResolveType(
    /* [in] */ const String& fullName)
{
    MetaResolver resolver(this, mMetaComponent);
    return resolver.Resolve(fullName);
}

String Module::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append("Module[");
    builder.Append("name:").Append(mName);
    builder.Append(", uuid:").Append(mUuid.Dump());
    builder.Append(", url:").Append(mUrl);
    if (!mDescription.IsNullOrEmpty()) {
        builder.Append(", description:").Append(mDescription);
    }
    builder.Append("]\n");
    builder.Append(Pool::Dump(prefix));
    return builder.ToString();
}

}
}
