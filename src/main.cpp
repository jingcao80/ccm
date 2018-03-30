
#include "Options.h"
#include "codegen/CodeGenerator.h"
#include "metadata/Component.h"
#include "metadata/MetaBuilder.h"
#include "metadata/MetaDumper.h"
#include "metadata/MetaSerializer.h"
#include "parser/Parser.h"
#include "util/File.h"
#include "util/Logger.h"
#include "util/String.h"

#include <memory>

using ccm::File;
using ccm::Logger;
using ccm::Options;
using ccm::Parser;
using ccm::String;
using ccm::codegen::CodeGenerator;
using ccm::metadata::MetaComponent;
using ccm::metadata::MetaBuilder;
using ccm::metadata::MetaDumper;
using ccm::metadata::MetaSerializer;

int main(int argc, char** argv)
{
    Options options(argc, argv);

    if (options.IsFormatError() || options.GetOptionNumber() <= 1
        || options.ShouldShowUsage()) {
        options.ShowUsage();
        return 0;
    }

    Parser parser;
    if (!parser.Parse(options.GetInputFile())) {
        Logger::E("ccdl", "Parsing failed.");
        return -1;
    }

    MetaBuilder mbuilder(parser.GetModule());
    if (!mbuilder.IsValidate()) {
        Logger::E("ccdl", "Parsing result is not validate.");
        return -1;
    }
    std::shared_ptr<MetaComponent> comMetadata = mbuilder.Build();

    MetaDumper dumper(comMetadata.get());
    String dumpStr = dumper.Dump();
    printf("%s", dumpStr.string());

    MetaSerializer serializer(comMetadata.get());
    serializer.Serialize();

    printf("==========================\n\n\n");

    int dataSize = serializer.GetDataSize();
    void* newData = malloc(dataSize);
    if (newData == nullptr) {
        Logger::E("ccdl", "Out of memory.");
        return -1;
    }

    uintptr_t data = serializer.GetData();
    memcpy(newData, reinterpret_cast<void*>(data), dataSize);
    serializer.Deserialize(reinterpret_cast<uintptr_t>(newData));

    dumper.Reset(reinterpret_cast<MetaComponent*>(newData));
    dumpStr = dumper.Dump();
    printf("%s", dumpStr.string());

    File output(options.GetOutputFile(), File::WRITE);

    output.Write(newData, dataSize);
    output.Flush();
    output.Close();

    CodeGenerator cg;
    cg.SetDirectory(options.GetOutputDir());
    cg.SetMetadata(reinterpret_cast<MetaComponent*>(newData));
    cg.Generate();

    return 0;
}
