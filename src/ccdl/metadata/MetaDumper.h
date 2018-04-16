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

#ifndef __CCDL_METADATA_METADUMPER_H__
#define __CCDL_METADATA_METADUMPER_H__

#include "../../runtime/metadata/Component.h"
#include "../util/String.h"

using ccm::metadata::MetaCoclass;
using ccm::metadata::MetaComponent;
using ccm::metadata::MetaConstant;
using ccm::metadata::MetaEnumeration;
using ccm::metadata::MetaInterface;
using ccm::metadata::MetaMethod;
using ccm::metadata::MetaNamespace;
using ccm::metadata::MetaParameter;
using ccm::metadata::MetaType;

namespace ccdl {
namespace metadata {

class MetaDumper
{
public:
    MetaDumper();

    String Dump(
        /* [in] */ MetaComponent* mc);

private:
    String DumpMetaComponent(
        /* [in] */ MetaComponent* mc,
        /* [in] */ const String& prefix);

    String DumpMetaCoclass(
        /* [in] */ MetaCoclass* mc,
        /* [in] */ const String& prefix);

    String DumpMetaConstant(
        /* [in] */ MetaConstant* mc,
        /* [in] */ const String& prefix);

    String DumpConstantValue(
        /* [in] */ MetaConstant* mc);

    String DumpMetaEnumeration(
        /* [in] */ MetaEnumeration* me,
        /* [in] */ const String& prefix);

    String DumpMetaInterface(
        /* [in] */ MetaInterface* mi,
        /* [in] */ const String& prefix);

    String DumpMetaMethod(
        /* [in] */ MetaMethod* mm,
        /* [in] */ const String& prefix);

    String DumpMetaNamespace(
        /* [in] */ MetaNamespace* mn,
        /* [in] */ const String& prefix);

    String DumpMetaParameter(
        /* [in] */ MetaParameter* mp);

    String DumpMetaType(
        /* [in] */ MetaType* mt,
        /* [in] */ int attr = 0);

private:
    MetaComponent* mMetaComponet;
};

}
}

#endif // __CCDL_METADATA_METADUMPER_H__
