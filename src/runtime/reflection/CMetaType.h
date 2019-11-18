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

#ifndef __COMO_CMETATYPE_H__
#define __COMO_CMETATYPE_H__

#include "comotypes.h"
#include "metadata/Component.h"
#include "util/comoref.h"

namespace como {

class CMetaType
    : public LightRefBase
    , public IMetaType
{
public:
    CMetaType() = default;

    CMetaType(
        /* [in] */ MetaComponent* mc,
        /* [in] */ MetaType* mt);

    COMO_INTERFACE_DECL();

    ECode GetName(
        /* [out] */ String& name);

    ECode GetTypeKind(
        /* [out] */ TypeKind& kind);

    ECode GetElementType(
        /* [out] */ AutoPtr<IMetaType>& elemType);

    ECode GetTypeModification(
        /* [out] */ TypeModification& mode);

private:
    String BuildName(
        /* [in] */ MetaComponent* mc,
        /* [in] */ MetaType* mt);

public:
    MetaType* mMetadata = nullptr;
    TypeKind mKind = TypeKind::Unknown;
    String mName;
    AutoPtr<IMetaType> mElementType;
    TypeModification mMode = TypeModification::NAKED;

private:
    static const char* TAG;
};

} // namespace como

#endif // __COMO_CMETATYPE_H__
