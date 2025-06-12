#include "backend.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/TargetParser/Host.h"
#include <optional>

namespace backend {

bool compile(const std::string& inputFile, const std::string& outputFile, bool emitAsm) {
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    llvm::InitializeAllAsmParsers();

    std::string targetTriple = llvm::sys::getProcessTriple();

    // Get the target
    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    if (!target) {
        llvm::errs() << error;
        return false;
    }

    // Create target machine
    auto CPU = "generic";
    auto features = "";
    llvm::TargetOptions opt;
    std::optional<llvm::Reloc::Model> RM;
    auto targetMachine = target->createTargetMachine(targetTriple, CPU, features, opt, RM);

    // Read the input LLVM IR file
    llvm::LLVMContext context;
    llvm::SMDiagnostic diag;
    std::unique_ptr<llvm::Module> module = llvm::parseIRFile(inputFile, diag, context);
    if (!module) {
        diag.print("backend", llvm::errs());
        return false;
    }

    module->setTargetTriple(targetTriple);

    // Create output file
    std::error_code EC;
    llvm::raw_fd_ostream dest(outputFile, EC, llvm::sys::fs::OF_None);
    if (EC) {
        llvm::errs() << "Could not open file: " << EC.message();
        return false;
    }

    llvm::legacy::PassManager pass;
    auto fileType = emitAsm ? llvm::CodeGenFileType::AssemblyFile : llvm::CodeGenFileType::ObjectFile;

    // Add passes to generate object file or assembly
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        llvm::errs() << "TargetMachine can't emit a file of this type";
        return false;
    }

    pass.run(*module);
    dest.flush();

    return true;
}

}
