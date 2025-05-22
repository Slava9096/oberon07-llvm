#pragma once

#include "llvm/IR/Module.h"
#include <string>

bool generateObjectFile(llvm::Module &module, const std::string &outputFilename, const std::string &targetTriple = "");
