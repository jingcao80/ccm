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

#ifndef __CCDL_CODEGEN_CODEGENERATOR_H__
#define __CCDL_CODEGEN_CODEGENERATOR_H__

#include "../../runtime/metadata/Component.h"
#include "../util/String.h"

using ccm::metadata::MetaComponent;
using ccm::metadata::MetaCoclass;
using ccm::metadata::MetaConstant;
using ccm::metadata::MetaEnumeration;
using ccm::metadata::MetaInterface;
using ccm::metadata::MetaMethod;
using ccm::metadata::MetaNamespace;
using ccm::metadata::MetaParameter;
using ccm::metadata::MetaType;
using ccm::metadata::MetaValue;

namespace ccdl {
namespace codegen {

class CodeGenerator
{
public:
    CodeGenerator();

    void GenerateOnCcmrtMode();

    void GenerateOnComponentMode();

    void GenerateOnUserMode();

    inline void SetDirectory(
        /* [in] */ const String& dir);

    inline void SetMetadata(
        /* [in] */ MetaComponent* component);

    void SetLicense(
        /* [in] */ const String& license);

    inline void SetSparseMode(
        /* [in] */ bool sparseMode);

private:
    bool ResolveDirectory();

    void GenConstantsAndTypesOnCcmrtMode();

    String GenConstantsInHeader(
        /* [in] */ MetaNamespace* mn);

    String GenConstant(
        /* [in] */ MetaConstant* mc);

    String GenEnumerationPredeclarations(
        /* [in] */ MetaNamespace* mn);

    String GenEnumerationDeclarations(
        /* [in] */ MetaNamespace* mn);

    String GenEnumerationDeclaration(
        /* [in] */ MetaEnumeration* me);

    String GenInterfaceIDPredeclarations(
        /* [in] */ MetaNamespace* mn);

    String GenInterfacePredeclarations(
        /* [in] */ MetaNamespace* mn);

    void GenInterfaceDeclarationsSparsely();

    void GenInterfaceDeclarationSparsely(
        /* [in] */ MetaInterface* mi);

    String GenIncludeForUsingNestedInterface(
        /* [in] */ MetaInterface* mi);

    String GenIncludeAutoPtr(
        /* [in] */ MetaInterface* mi);

    String GenInterfaceDeclarations(
        /* [in] */ MetaNamespace* mn);

    String GenInterfaceDeclaration(
        /* [in] */ MetaInterface* mi,
        /* [in] */ const String& prefix);

    String GenInterfaceConstant(
        /* [in] */ MetaConstant* mc,
        /* [in] */ const String& prefix);

    String GenInterfaceMethod(
        /* [in] */ MetaMethod* mm,
        /* [in] */ const String& prefix);

    String GenParameter(
        /* [in] */ MetaParameter* mp);

    String GenType(
        /* [in] */ MetaType* mt,
        /* [in] */ int attr = 0,
        /* [in] */ bool inArray = false);

    String GenValue(
        /* [in] */ MetaType* mt,
        /* [in] */ MetaValue& mv);

    void GenIds();

    String GenComponentID();

    String GenInterfaceIDsOnCcmrtMode(
        /* [in] */ MetaNamespace* mn);

    void GenConstantsAndTypesOnComponentMode();

    String GenNamespaceBegin(
        /* [in] */ const String& ns);

    String GenNamespaceEnd(
        /* [in] */ const String& ns);

    void GenCoclasses();

    void GenCoclass(
        /* [in] */ MetaCoclass* mk);

    void GenCoclassHeader(
        /* [in] */ MetaCoclass* mk);

    void GenCoclassCpp(
        /* [in] */ MetaCoclass* mk);

    String GenCoclassObject(
        /* [in] */ MetaCoclass* mk);

    String GenCoclassMethods(
        /* [in] */ MetaCoclass* mk);

    void GenComponentCpp();

    String GenConstantsInCpp(
        /* [in] */ MetaNamespace* mn);

    String GenInterfaceConstantsInCpp(
        /* [in] */ MetaNamespace* mn);

    String GenInterfaceIDsInCpp(
        /* [in] */ MetaNamespace* mn);

    String GenCoclassIDsInCpp(
        /* [in] */ MetaNamespace* mn);

    String GenClassObjectGetterArray();

    String GenSoGetAllClassObjects();

    String GenSoGetClassObject();

    String GenComponentObject();

    void GenConstantsAndTypesOnUserMode();

    String GenCoclassDeclarations(
        /* [in] */ MetaNamespace* mn);

    String GenCoclassDeclaration(
        /* [in] */ MetaCoclass* mc);

    void GenCoclassDeclarationsSparselyOnUserMode();

    void GenCoclassDeclarationSparselyOnUserMode(
        /* [in] */ MetaCoclass* mc);

    void GenComponentCppOnUserMode();

    String GenCoclassesOnUserMode(
        /* [in] */ MetaNamespace* mn);

    String GenCoclassOnUserMode(
        /* [in] */ MetaCoclass* mc);

    String GenDefineMacro(
        /* [in] */ const String& fullName);

    void GenMetadataWrapper();

private:
    static const String TAG;
    static const String TAB;
    String mDirectory;
    MetaComponent* mMetaComponent;
    String mLicense;
    bool mSparseMode;
};

void CodeGenerator::SetDirectory(
    /* [in] */ const String& dir)
{
    mDirectory = dir;
}

void CodeGenerator::SetMetadata(
    /* [in] */ MetaComponent* component)
{
    mMetaComponent = component;
}

void CodeGenerator::SetSparseMode(
    /* [in] */ bool sparseMode)
{
    mSparseMode = sparseMode;
}

}
}

#endif // __CCDL_CODEGEN_CODEGENERATOR_H__
