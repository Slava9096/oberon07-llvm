#include "middleend.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Passes/OptimizationLevel.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include <fcntl.h>

namespace middleend {

bool optimize(const std::string& inputFile, int optLevel) {
    llvm::LLVMContext context;
    llvm::SMDiagnostic error;

    // Read the input LLVM IR file
    std::unique_ptr<llvm::Module> module = llvm::parseIRFile(inputFile, error, context);
    if (!module) {
        error.print("middleend", llvm::errs());
        return false;
    }

    llvm::OptimizationLevel level;
    switch (optLevel) {
        case 0: level = llvm::OptimizationLevel::O0; break;
        case 1: level = llvm::OptimizationLevel::O1; break;
        case 2: level = llvm::OptimizationLevel::O2; break;
        case 3: level = llvm::OptimizationLevel::O3; break;
        default: return false;
    }

    llvm::PassBuilder passBuilder;
    llvm::LoopAnalysisManager loopAnalysisManager;
    llvm::FunctionAnalysisManager functionAnalysisManager;
    llvm::CGSCCAnalysisManager cgsccAnalysisManager;
    llvm::ModuleAnalysisManager moduleAnalysisManager;

    passBuilder.registerModuleAnalyses(moduleAnalysisManager);
    passBuilder.registerCGSCCAnalyses(cgsccAnalysisManager);
    passBuilder.registerFunctionAnalyses(functionAnalysisManager);
    passBuilder.registerLoopAnalyses(loopAnalysisManager);
    passBuilder.crossRegisterProxies(loopAnalysisManager, functionAnalysisManager, cgsccAnalysisManager, moduleAnalysisManager);

    llvm::ModulePassManager modulePassManager = passBuilder.buildPerModuleDefaultPipeline(level);
    modulePassManager.run(*module, moduleAnalysisManager);

    // Write the optimized IR back to the file
    std::error_code EC;
    int fd = open(inputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        llvm::errs() << "Error opening output file\n";
        return false;
    }
    llvm::raw_fd_ostream outFile(fd, true);
    if (outFile.has_error()) {
        llvm::errs() << "Error creating output stream\n";
        return false;
    }

    module->print(outFile, nullptr);
    return true;
}

}
