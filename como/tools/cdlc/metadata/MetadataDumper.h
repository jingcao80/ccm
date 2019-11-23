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

#ifndef __CDLC_METADATADUMPER_H__
#define __CDLC_METADATADUMPER_H__

#include "metadata/Metadata.h"
#include "util/String.h"

namespace cdlc {

class MetadataDumper
{
public:
    inline MetadataDumper(
        /* [in] */ como::MetaComponent* mc);

    String Dump(
        /* [in] */ const String& prefix);

private:
    String DumpMetaComponent(
        /* [in] */ como::MetaComponent* mc,
        /* [in] */ const String& prefix);

    String DumpMetaNamespace(
        /* [in] */ como::MetaNamespace* mn,
        /* [in] */ const String& prefix);

    String DumpMetaConstant(
        /* [in] */ como::MetaConstant* mc,
        /* [in] */ const String& prefix);

    String DumpMetaEnumeration(
        /* [in] */ como::MetaEnumeration* me,
        /* [in] */ const String& prefix);

    String DumpMetaCoclass(
        /* [in] */ como::MetaCoclass* mk,
        /* [in] */ const String& prefix);

    String DumpMetaInterface(
        /* [in] */ como::MetaInterface* mi,
        /* [in] */ const String& prefix);

    String DumpMetaMethod(
        /* [in] */ como::MetaMethod* mm,
        /* [in] */ const String& prefix);

    String DumpMetaParameter(
        /* [in] */ como::MetaParameter* mp,
        /* [in] */ const String& prefix);

    String DumpMetaType(
        /* [in] */ como::MetaType* mt);

    String DumpMetaValue(
        /* [in] */ como::MetaType* mt,
        /* [in] */ como::MetaValue* mv);

    String DumpUUID(
        /* [in] */ const como::UUID& uuid);

private:
    como::MetaComponent* mComponent = nullptr;
};

MetadataDumper::MetadataDumper(
    /* [in] */ como::MetaComponent* mc)
    : mComponent(mc)
{}

}

#endif // __CDLC_METADATADUMPER_H__
