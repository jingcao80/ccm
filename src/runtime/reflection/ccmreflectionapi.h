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

#ifndef __CCM_CCMREFLECTIONAPI_H__
#define __CCM_CCMREFLECTIONAPI_H__

#include "ccmdef.h"
#include "ccmtypes.h"

namespace ccm {

EXTERN_C COM_PUBLIC ECode CoGetComponentMetadata(
    /* [in] */ const ComponentID& cid,
    /* [in] */ IClassLoader* loader,
    /* [out] */ IMetaComponent** mc);

EXTERN_C COM_PUBLIC ECode CoGetComponentMetadataFromFile(
    /* [in] */ HANDLE fd,
    /* [in] */ IClassLoader* loader,
    /* [out] */ IMetaComponent** mc);

EXTERN_C COM_PUBLIC ECode CoGetCoclassMetadata(
    /* [in] */ const CoclassID& cid,
    /* [in] */ IClassLoader* loader,
    /* [in] */ IMetaCoclass** mc);

}

#endif // __CCM_CCMREFLECTIONAPI_H__
