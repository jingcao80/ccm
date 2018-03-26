
#include "Options.h"
#include "metadata/Component.h"
#include "metadata/MetaBuilder.h"
#include "parser/Parser.h"
#include "util/File.h"
#include "util/Logger.h"

#include <memory>

using ccm::File;
using ccm::Logger;
using ccm::Options;
using ccm::Parser;
using ccm::metadata::MetaComponent;
using ccm::metadata::MetaBuilder;

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

    return 0;
}
