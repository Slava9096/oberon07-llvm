#include "backend.h"

// #include "/opt/llvm-project/llvm/include/llvm/Target/TargetMachine.h"
// #include "/opt/llvm-project/llvm/include/llvm/Target/TargetOptions.h"
// #include "/opt/llvm-project/llvm/include/llvm/Support/FileSystem.h"
// #include "/opt/llvm-project/llvm/include/llvm/IR/LegacyPassManager.h"
// #include "/opt/llvm-project/llvm/include/llvm/MC/TargetRegistry.h"
// #include "/opt/llvm-project/llvm/include/llvm/Support/CodeGen.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

bool generateObjectFile(llvm::Module &module, const std::string &outputFilename, const std::string &targetTriple)
{
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    std::string error;
    const llvm::Target *target = llvm::TargetRegistry::lookupTarget(targetTriple, error);

    if (!target) {
        llvm::errs() << "Can't find target: " << error << "\n";
        return false;
    }

    llvm::TargetOptions options;
    std::unique_ptr<llvm::TargetMachine> targetMachine(
        target->createTargetMachine(
            targetTriple,
            "generic",
            "",
            options,
            llvm::Reloc::Model::PIC_
        )
    );

    module.setDataLayout(targetMachine->createDataLayout());
    module.setTargetTriple(targetTriple);

    std::error_code EC;
    llvm::raw_fd_ostream dest(outputFilename, EC, llvm::sys::fs::OF_None);

    if (EC) {
        llvm::errs() << "Could not open destination file: " << EC.message() << "\n";
        return false;
    }

    llvm::legacy::PassManager codegenPM;

    if (targetMachine->addPassesToEmitFile(codegenPM, dest, nullptr, llvm::CodeGenFileType::ObjectFile)) {
        llvm::errs() << "TargetMachine can't emit object file\n";
        return false;
    }

    codegenPM.run(module);
    dest.flush();

    return true;
}
