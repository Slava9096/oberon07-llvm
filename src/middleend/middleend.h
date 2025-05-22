#pragma once

#include "llvm/IR/Module.h"
#include "llvm/Passes/OptimizationLevel.h"
#include "llvm/Passes/PassBuilder.h"

void optimize(llvm::Module& module, llvm::OptimizationLevel optimizationLevel);
