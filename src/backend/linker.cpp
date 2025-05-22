#include "linker.h"
#include <cstdlib>
#include "llvm/Support/raw_ostream.h"

bool linkObjectFile(const std::string &objectFilename, const std::string &outputExecutable)
{
    std::string command = "clang " + objectFilename + " -o " + outputExecutable;

    llvm::outs() << "Running linker command: " << command << "\n";

    int result = std::system(command.c_str());

    if (result != 0) {
        llvm::errs() << "Linking failed\n";
        return false;
    }

    llvm::outs() << "Executable generated: " << outputExecutable << "\n";
    return true;
}
