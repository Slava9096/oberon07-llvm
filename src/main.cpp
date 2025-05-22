#include "backend/backend.h"
#include "backend/linker.h"
#include "middleend/middleend.h"

// #include "/opt/llvm-project/llvm/include/llvm/IR/Module.h"
// #include "/opt/llvm-project/llvm/include/llvm/IR/LLVMContext.h"
// #include "/opt/llvm-project/llvm/include/llvm/IRReader/IRReader.h"
// #include "/opt/llvm-project/llvm/include/llvm/Passes/OptimizationLevel.h"
// #include "/opt/llvm-project/llvm/include/llvm/Support/SourceMgr.h"

#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Passes/OptimizationLevel.h"
#include "llvm/Support/SourceMgr.h"
#include <llvm/Support/raw_ostream.h>

int main(int argc, char* argv[]){
    llvm::OptimizationLevel optimizationLevel = llvm::OptimizationLevel::O2;
    switch (argv[1][2]) {
        case '0': optimizationLevel = llvm::OptimizationLevel::O0; break;
        case '1': optimizationLevel = llvm::OptimizationLevel::O1; break;
        case '2': optimizationLevel = llvm::OptimizationLevel::O2; break;
        case '3': optimizationLevel = llvm::OptimizationLevel::O3; break;
        default: return 1;
    }

    llvm::LLVMContext context;
    llvm::SMDiagnostic error;

    std::unique_ptr<llvm::Module> module = llvm::parseIRFile("test/input.ll", error, context);

    if (!module) {
        error.print("my_compiler", llvm::errs());
        return 1;
    }

    optimize(*module, optimizationLevel);

    llvm::raw_ostream& out = llvm::outs();
    module->print(out, nullptr);
    out.flush();

    generateObjectFile(*module, "output_x86_64.o", "x86_64-pc-linux-gnu");
    llvm::outs() << "Object files generated successfully.\n";

    linkObjectFile("output_x86_64.o", "my_program");

    return 0;
}
