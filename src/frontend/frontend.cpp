#include <fstream>
#include "frontend.h"
#include "foolexer.h"
#include "ast/base.h"
#include "parser.hpp"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"

namespace frontend {

bool compile(const std::string& inputFile, const std::string& outputFile, bool debug) {
    std::ifstream src(inputFile);
    if (!src.is_open()) {
        return false;
    }

    Statement* program = nullptr;
    FooLexer* lexer = new FooLexer(src, std::cerr);
    lexer->setDebugMode(debug);
    yy::parser* parser = new yy::parser(&program, lexer);

    if (parser->parse() != 0) {
        delete program;
        delete parser;
        delete lexer;
        return false;
    }

    llvm::LLVMContext llvmContext;
    std::unique_ptr<llvm::Module> module = std::make_unique<llvm::Module>("main", llvmContext);
    llvm::IRBuilder<> builder(llvmContext);

    // Create main function
    llvm::FunctionType* mainType = llvm::FunctionType::get(builder.getInt32Ty(), false);
    llvm::Function* mainFunc = llvm::Function::Create(mainType, llvm::Function::ExternalLinkage, "main", module.get());
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(llvmContext, "entry", mainFunc);
    builder.SetInsertPoint(entry);

    SymbolTable* symbolTable = new SymbolTable();
    program->codegen(&llvmContext, builder, symbolTable);
    delete symbolTable;

    // Add return 0 to main
    builder.CreateRet(builder.getInt32(0));

    // Verify the module
    std::string error;
    llvm::raw_string_ostream errorStream(error);
    if (llvm::verifyModule(*module, &errorStream)) {
        std::cerr << "Error verifying module: " << error << std::endl;
        delete program;
        delete parser;
        delete lexer;
        return false;
    }

    // Write to file
    std::error_code EC;
    llvm::raw_fd_ostream outFile(outputFile, EC, llvm::sys::fs::OF_None);
    if (EC) {
        std::cerr << "Error opening output file: " << EC.message() << std::endl;
        delete program;
        delete parser;
        delete lexer;
        return false;
    }

    module->print(outFile, nullptr);
    outFile.close();

    delete program;
    delete parser;
    delete lexer;
    return true;
}

bool interpret(const std::string& inputFile, bool debug) {
    std::ifstream src(inputFile);
    if (!src.is_open()) {
        return false;
    }

    Statement* program = nullptr;
    FooLexer* lexer = new FooLexer(src, std::cerr);
    lexer->setDebugMode(debug);
    yy::parser* parser = new yy::parser(&program, lexer);

    if (parser->parse() != 0) {
        delete program;
        delete parser;
        delete lexer;
        return false;
    }

    Context* context = new Context();
    program->Execute(context);
    delete context;

    delete program;
    delete parser;
    delete lexer;
    return true;
}

}
