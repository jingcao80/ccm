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

#ifndef __CDLC_CODEGENERATOR_H__
#define __CDLC_CODEGENERATOR_H__

#include "metadata/Metadata.h"
#include "util/LightRefBase.h"
#include "util/String.h"

namespace cdlc {

class CodeGenerator
{
private:
    class Emitter
        : public LightRefBase
    {
    public:
        inline Emitter(
            /* [in] */ CodeGenerator* owner)
            : mOwner(owner)
            , mComponent(mOwner->mComponent)
        {}

        virtual void Emit() = 0;

    protected:
        void EmitInterfaceDeclarationsSplitly();

        void EmitInterfaceDeclarationSplitly(
            /* [in] */ como::MetaInterface* mi);

        String EmitIncludeForUsingNestedInterface(
            /* [in] */ como::MetaInterface* mi);

        String EmitConstantsAndTypeForwardDeclarationsRecursively(
            /* [in] */ como::MetaNamespace* mn);

        String EmitConstantsAndTypeDeclarationsRecursively(
            /* [in] */ como::MetaNamespace* mn);

        String EmitNamespaceBegin(
            /* [in] */ const String& nsStr);

        String EmitNamespaceEnd(
            /* [in] */ const String& nsStr);

        String EmitConstantsInHeader(
            /* [in] */ como::MetaNamespace* mn);

        String EmitConstantForHeader(
            /* [in] */ como::MetaConstant* mc);

        String EmitEnumerationForwardDeclarations(
            /* [in] */ como::MetaNamespace* mn);

        String EmitEnumerationDeclarations(
            /* [in] */ como::MetaNamespace* mn);

        String EmitEnumerationDeclaration(
            /* [in] */ como::MetaEnumeration* me);

        String EmitInterfaceIDForwardDeclarations(
            /* [in] */ como::MetaNamespace* mn);

        String EmitInterfaceForwardDeclarations(
            /* [in] */ como::MetaNamespace* mn);

        String EmitInterfaceDeclarations(
            /* [in] */ como::MetaNamespace* mn);

        String EmitInterfaceDeclaration(
            /* [in] */ como::MetaInterface* mi,
            /* [in] */ const String& prefix);

        String EmitInterfaceConstant(
            /* [in] */ como::MetaConstant* mc,
            /* [in] */ const String& prefix);

        String EmitInterfaceMethod(
            /* [in] */ como::MetaMethod* mm,
            /* [in] */ const String& prefix);

        String EmitParameter(
            /* [in] */ como::MetaParameter* mp);

        String EmitType(
            /* [in] */ como::MetaType* mt,
            /* [in] */ int mode);

        String EmitValue(
            /* [in] */ como::MetaType* mt,
            /* [in] */ como::MetaValue* mv);

        String EmitConstantsAndTypesRecursivelyInCpp(
            /* [in] */ como::MetaNamespace* mn);

        String EmitConstantsInCpp(
            /* [in] */ como::MetaNamespace* mn);

        String EmitInterfaceIDsInCpp(
            /* [in] */ como::MetaNamespace* mn);

        String EmitInterfaceConstantsInCpp(
            /* [in] */ como::MetaNamespace* mn);

        String EmitCoclassIDsInCpp(
            /* [in] */ como::MetaNamespace*  mn);

        String EmitComponentID();

        String EmitDefineMacro(
            /* [in] */ const String& fullName);

        String CanonicalizeNamespace(
            /* [in] */ const String& ns);

        String ConcatString(
            /* [in] */ const String& string1,
            /* [in] */ const String& string2);

        void EmitMetadataWrapper();

    protected:
        static constexpr int MODE_VARIABLE = 0;
        static constexpr int MODE_PARAMETER_IN = PARAMETER_IN;
        static constexpr int MODE_PARAMETER_OUT = PARAMETER_OUT;
        static constexpr int MODE_PARAMETER_CALLEE = PARAMETER_CALLEE;

        CodeGenerator* mOwner = nullptr;
        como::MetaComponent* mComponent = nullptr;
    };

    class ComponentModeEmitter
        : public Emitter
    {
    public:
        inline ComponentModeEmitter(
            /* [in] */ CodeGenerator* owner)
            : Emitter(owner)
        {}

        void Emit() override;

    private:
        void EmitConstantsAndTypes();

        void EmitCoclasses();

        void EmitCoclass(
            /* [in] */ como::MetaCoclass* mk);

        void EmitCoclassHeader(
            /* [in] */ como::MetaCoclass* mk);

        void EmitCoclassCpp(
            /* [in] */ como::MetaCoclass* mk);

        String EmitCoclassObject(
            /* [in] */ como::MetaCoclass* mk);

        String EmitCoclassMethods(
            /* [in] */ como::MetaCoclass* mk);

        void EmitComponentCpp();

        String EmitClassObjectGetterArray();

        String EmitGetClassObjectFuncRecursively(
            /* [in] */ como::MetaNamespace* mn);

        String EmitSoGetAllClassObjects();

        String EmitSoGetClassObject();

        String EmitComponentObject();
    };

    class ClientModeEmitter
        : public Emitter
    {
    public:
        inline ClientModeEmitter(
            /* [in] */ CodeGenerator* owner)
            : Emitter(owner)
        {}

        void Emit() override;

    private:
        void EmitConstantsAndTypes();

        String EmitConstantsAndTypeForwardDeclarationsRecursively(
            /* [in] */ como::MetaNamespace* mn);

        String EmitConstantsAndTypeDeclarationsRecursively(
            /* [in] */ como::MetaNamespace* mn);

        String EmitCoclassDeclarations(
            /* [in] */ como::MetaNamespace* mn);

        String EmitCoclassDeclaration(
            /* [in] */ como::MetaCoclass* mk);

        void EmitCoclassDeclarationsSplitly();

        void EmitCoclassDeclarationSplitly(
            /* [in] */ como::MetaCoclass* mk);

        void EmitComponentCpp();

        String EmitConstantsAndTypesRecursivelyInCpp(
            /* [in] */ como::MetaNamespace* mn);

        String EmitCoclasses(
            /* [in] */ como::MetaNamespace* mn);

        String EmitCoclass(
            /* [in] */ como::MetaCoclass* mk);
    };

    class RuntimeModeEmitter
        : public Emitter
    {
    public:
        inline RuntimeModeEmitter(
            /* [in] */ CodeGenerator* owner)
            : Emitter(owner)
        {}

        void Emit() override;

    private:
        void EmitConstantsAndTypes();

        void EmitUUIDs();
    };

public:
    CodeGenerator();

    void Generate();

    inline void SetMetadata(
        /* [in] */ como::MetaComponent* component);

    inline void SetDirectory(
        /* [in] */ const String& directory);

    inline void SetMode(
        /* [in] */ int mode);

private:
    bool CheckDirectory();

private:
    static const char* TAG;

    como::MetaComponent* mComponent = nullptr;
    String mDirectory;
    int mMode;
    String mLicense;
};

void CodeGenerator::SetMetadata(
    /* [in] */ como::MetaComponent* component)
{
    mComponent = component;
}

void CodeGenerator::SetDirectory(
    /* [in] */ const String& directory)
{
    mDirectory = directory;
}

void CodeGenerator::SetMode(
    /* [in] */ int mode)
{
    mMode = mode;
}

}

#endif // __CDLC_CODEGENERATOR_H__

