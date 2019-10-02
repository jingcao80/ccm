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
        String EmitConstantsInHeader(
            /* [in] */ como::MetaNamespace* mn);

        String EmitEnumerationForwardDeclarations(
            /* [in] */ como::MetaNamespace* mn);

        String EmitEnumerationDeclarations(
            /* [in] */ como::MetaNamespace* mn);

        String EmitInterfaceIDForwardDeclarations(
            /* [in] */ como::MetaNamespace* mn);

        String EmitInterfaceForwardDeclarations(
            /* [in] */ como::MetaNamespace* mn);

        String EmitInterfaceDeclarations(
            /* [in] */ como::MetaNamespace* mn);

        String EmitConstantsInCpp(
            /* [in] */ como::MetaNamespace* mn);

        String EmitInterfaceIDsInCpp(
            /* [in] */ como::MetaNamespace* mn);

    private:
        String EmitConstantForHeader(
            /* [in] */ como::MetaConstant* mc);

        String EmitEnumerationDeclaration(
            /* [in] */ como::MetaEnumeration* me);

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

        void EmitMetadataWrapper();
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

public:
    static constexpr int MODE_MASK = 0x07;
    static constexpr int MODE_CLIENT = 0x01;
    static constexpr int MODE_COMPONENT = 0x02;
    static constexpr int MODE_RUNTIME = 0x04;
    static constexpr int INTERFACE_SPLIT = 0x10;

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

