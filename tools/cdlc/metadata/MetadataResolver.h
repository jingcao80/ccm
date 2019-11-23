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

#ifndef __CDLC_METADATARESOLVER_H__
#define __CDLC_METADATARESOLVER_H__

#include "ast/Expression.h"
#include "ast/Method.h"
#include "ast/Module.h"
#include "ast/Namespace.h"
#include "ast/Parameter.h"
#include "metadata/Metadata.h"

namespace cdlc {

class MetadataResolver
{
public:
    inline MetadataResolver(
        /* [in] */ Module* module,
        /* [in] */ como::MetaComponent* mc);

    AutoPtr<Type> ResolveType(
        /* [in] */ const String& typeName,
        /* [in] */ Namespace* ns,
        /* [in] */ como::MetaNamespace* mn);

private:
    AutoPtr<Type> BuildEnumeration(
        /* [in] */ Namespace* ns,
        /* [in] */ como::MetaEnumeration* me);

    AutoPtr<Type> BuildInterface(
        /* [in] */ Namespace* ns,
        /* [in] */ como::MetaInterface* mi);

    AutoPtr<Method> BuildMethod(
        /* [in] */ como::MetaMethod* mm);

    AutoPtr<Parameter> BuildParameter(
        /* [in] */ como::MetaParameter* mp);

    AutoPtr<Type> BuildType(
        /* [in] */ como::MetaType* mt);

    AutoPtr<Expression> BuildValue(
        /* [in] */ Type* type,
        /* [in] */ como::MetaValue* mv);

private:
    Module* mModule = nullptr;
    como::MetaComponent* mComponent = nullptr;

    String mResolvingTypename;
    AutoPtr<Type> mResolvingType;
};

MetadataResolver::MetadataResolver(
    /* [in] */ Module* module,
    /* [in] */ como::MetaComponent* mc)
    : mModule(module)
    , mComponent(mc)
{}

}

#endif // __CDLC_METADATARESOLVER_H__
