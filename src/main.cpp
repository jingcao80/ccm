
#include "Options.h"
#include "Parser.h"
#include "util/File.h"

#include <memory>

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

    std::shared_ptr<File> file(new File(options.GetInputFile()));

    Parser parser;
    parser.Parse(file);

    return 0;
}
