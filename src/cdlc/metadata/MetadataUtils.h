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

#ifndef __CDLC_METADATAUTILS_H__
#define __CDLC_METADATAUTILS_H__

#include "metadata/Metadata.h"
#include "util/String.h"

namespace cdlc {

class MetadataUtils
{
public:
    static void* ReadMetadata(
        /* [in] */ const String& filePath,
        /* [in] */ int fileType);

private:
    static void* ReadMetadataFromElf64(
        /* [in] */ const String& filePath);

    static void* ReadMetadataFromFile(
        /* [in] */ const String& filePath);

public:
    static constexpr int TYPE_METADATA = 1;
    static constexpr int TYPE_SO = 2;
};

}

#endif // __CDLC_METADATAUTILS_H__
