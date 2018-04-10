
#include "Options.h"
#include "codegen/CodeGenerator.h"
#include "metadata/MetaBuilder.h"
#include "metadata/MetaDumper.h"
#include "parser/Parser.h"
#include "util/File.h"
#include "util/Logger.h"
#include "util/MetadataUtils.h"
#include "util/String.h"
#include "../runtime/metadata/Component.h"
#include "../runtime/metadata/MetaSerializer.h"

#include <memory>

using ccdl::File;
using ccdl::Logger;
using ccdl::MetadataUtils;
using ccdl::Options;
using ccdl::Parser;
using ccdl::String;
using ccdl::codegen::CodeGenerator;
using ccdl::metadata::MetaBuilder;
using ccdl::metadata::MetaDumper;
using ccm::metadata::MetaComponent;
using ccm::metadata::MetaSerializer;

int main(int argc, char** argv)
{
    Options options(argc, argv);

    if (options.IsFormatError() || options.GetOptionNumber() <= 1
        || options.ShouldShowUsage()) {
        options.ShowUsage();
        return 0;
    }

    std::shared_ptr<MetaComponent> comMetadata;

    if (options.ShouldCompile()) {
        Parser parser;
        if (!parser.Parse(options.GetInputFile())) {
            Logger::E("ccdl", "Parsing failed.");
            return -1;
        }

        MetaBuilder mbuilder(parser.GetModule());
        if (!mbuilder.IsValid()) {
            Logger::E("ccdl", "Parsing result is not validate.");
            return -1;
        }

        comMetadata = mbuilder.Build();
        if (comMetadata == nullptr) {
            Logger::E("ccdl", "Generate metadata failed.");
            return -1;
        }

        if (options.ShouldSaveMetadata()) {
            File file(options.GetMetadataOutputFile(), File::WRITE);
            if (!file.IsValid()) {
                Logger::E("ccdl", "Create metadata file failed.");
                return -1;
            }

            MetaSerializer serializer(comMetadata.get());
            serializer.Serialize();
            int dataSize = serializer.GetDataSize();
            uintptr_t data = serializer.GetData();

            file.Write(reinterpret_cast<void*>(data), dataSize);
            file.Flush();
            file.Close();
        }
    }

    if (options.ShouldGenerate()) {
        if (comMetadata == nullptr) {
            String mdFile = options.GetInputFile();
            void* newData = nullptr;
            if (options.IsFromSoFile()) {
                newData = MetadataUtils::ReadMetadataFromElf64(mdFile);
            }
            else if (options.IsFromMetadataFile()) {
                newData = MetadataUtils::ReadMetadataFromFile(mdFile);
            }
            else {
                Logger::E("ccdl", "Do not support get metadata from file \"%s\".",
                        options.GetInputFile().string());
                return -1;
            }
            if (newData == nullptr) {
                Logger::E("ccdl", "Get metadata from \"%s\" failed.", mdFile.string());
                return -1;
            }

            MetaSerializer serializer;
            serializer.Deserialize(reinterpret_cast<uintptr_t>(newData));
            comMetadata.reset((MetaComponent*)newData);
        }

        CodeGenerator cg;
        cg.SetDirectory(options.GetOutputDir());
        cg.SetMetadata(comMetadata.get());
        if (options.IsGenForComponent()) {
            cg.GenerateForComponent();
        }
        else {
            cg.GenerateForComponentUser();
        }
    }

    return 0;
}
