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

#include "ast/Module.h"
#include "codegen/CodeGenerator.h"
#include "metadata/Metadata.h"
#include "metadata/MetadataBuilder.h"
#include "metadata/MetadataDumper.h"
#include "metadata/MetadataUtils.h"
#include "parser/Parser.h"
#include "util/File.h"
#include "util/Logger.h"
#include "util/Options.h"

using namespace cdlc;

static const char* TAG = "cdlc";

int main(int argc, char** argv)
{
    Options options(argc, argv);

    if (options.DoShowUsage()) {
        options.ShowUsage();
        return 0;
    }

    if (options.HasErrors()) {
        options.ShowErrors();
        return 0;
    }

    std::shared_ptr<como::MetaComponent> component;

    if (options.DoCompile()) {
        Parser parser;
        if (!parser.Parse(options.GetSourceFile())) {
            Logger::E(TAG, "Parsing failed.");
            return -1;
        }

        if (options.DoDumpAST()) {
            AutoPtr<Module> module = parser.GetCompiledModule();
            printf("%s", module->Dump("").string());
        }

        MetadataBuilder builder(parser.GetCompiledModule());
        component = builder.Build();
        if (component == nullptr) {
            Logger::E(TAG, "Generate metadata failed.");
            return -1;
        }

        if (options.DoDumpMetadata()) {
            MetadataDumper dumper(component.get());
            printf("%s", dumper.Dump("").string());
        }

        if (options.DoSaveMetadata()) {
            File file(options.GetSaveFile(), File::WRITE);
            if (!file.IsValid()) {
                Logger::E("cdlc", "Create metadata file \"%s\" failed.", file.GetPath().string());
                return -1;
            }

            como::metadata::MetadataSerializer serializer(component.get());
            serializer.Serialize();
            size_t metadataSize = serializer.GetSize();
            uintptr_t metadata = serializer.GetSerializedMetadata();

            if (!file.Write(reinterpret_cast<void*>(metadata), metadataSize)) {
                Logger::E("cdlc", "Write metadata file \"%s\" failed.", file.GetPath().string());
                return -1;
            }
            file.Flush();
            file.Close();

            component = nullptr;
        }
    }

    if (options.DoGenerateCode()) {
        if (component == nullptr) {
            void* metadata = MetadataUtils::ReadMetadata(
                    options.DoSaveMetadata()
                        ? options.GetSaveFile()
                        : options.GetMetadataFile(),
                    options.DoSaveMetadata()
                        ? MetadataUtils::TYPE_METADATA
                        : options.GetMetadataFileType());
            if (metadata == nullptr) {
                Logger::E("ccdl", "Read metadata from \"%s\" failed.",
                        options.GetMetadataFile().string());
                return -1;
            }

            como::metadata::MetadataSerializer serializer;
            serializer.Deserialize(reinterpret_cast<uintptr_t>(metadata));
            component.reset(
                    reinterpret_cast<como::MetaComponent*>(metadata),
                    [](como::MetaComponent* p){ free(p); });
        }

        CodeGenerator generator;
        generator.SetDirectory(options.GetCodegenDirectory());
        generator.SetMetadata(component.get());
        generator.SetMode(options.GetCodegenMode());
        generator.Generate();
    }

    return 0;
}
