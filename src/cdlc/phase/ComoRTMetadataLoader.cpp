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

#include "phase/ComoRTMetadataLoader.h"
#include "ast/Module.h"
#include "metadata/Metadata.h"
#include "metadata/MetadataUtils.h"
#include "util/Logger.h"

namespace cdlc {

const char* ComoRTMetadataLoader::TAG = "ComoRTMetadataLoader";

bool ComoRTMetadataLoader::Process()
{
    String rtPath(getenv("RT_PATH"));
    void* metadata = MetadataUtils::ReadMetadata(
            rtPath + "/comort.so", MetadataUtils::TYPE_SO);
    if (metadata == nullptr) {
        Logger::E(TAG, "Load metadata from comort failed.");
        return false;
    }

    como::MetadataSerializer serializer;
    serializer.Deserialize(reinterpret_cast<uintptr_t>(metadata));
    AutoPtr<Module> comort = Module::Resolve(metadata);
    mWorld.AddDependentModule(comort);
    return true;
}

}
