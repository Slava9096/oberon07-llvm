#include "linker.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdlib>
#include <filesystem>

namespace backend {

bool link(const std::string& objectFile, const std::string& outputFile) {
    std::string command = "clang -no-pie " + objectFile + " -o " + outputFile;
    llvm::outs() << "Running linker command: " << command << "\n";

    int result = std::system(command.c_str());
    if (result != 0) {
        llvm::errs() << "Error: Linking failed with exit code " << result << "\n";
        return false;
    }

    if (!std::filesystem::exists(outputFile)) {
        llvm::errs() << "Error: Output file was not created\n";
        return false;
    }

    llvm::outs() << "Successfully created executable: " << outputFile << "\n";
    return true;
}

}
