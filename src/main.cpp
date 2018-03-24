
#include "Options.h"
#include "metadata/Component.h"
#include "parser/Parser.h"
#include "util/File.h"

using ccm::File;
using ccm::Options;
using ccm::Parser;

int main(int argc, char** argv)
{
    Options options(argc, argv);

    if (options.IsFormatError() || options.GetOptionNumber() <= 1
        || options.ShouldShowUsage()) {
        options.ShowUsage();
        return 0;
    }

    Parser parser;
    parser.Parse(options.GetInputFile());

    return 0;
}
