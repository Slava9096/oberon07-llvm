#include <iostream>
#include <string>
#include <filesystem>
#include <getopt.h>
#include "frontend/frontend.h"
#include "middleend/middleend.h"
#include "backend/backend.h"
#include "backend/linker.h"
#include <llvm/Support/raw_ostream.h>

void printUsage(const char* programName) {
    std::cerr << "Usage: " << programName << " [options] -i <input_file>\n"
              << "Options:\n"
              << "  -i, --input <file>     Input Oberon source file\n"
              << "  -o, --output <file>    Output file (default: a.out)\n"
              << "  -O, --optimize <level> Optimization level (0-3, default: 0)\n"
              << "  --interpreter          Run in interpreter mode\n"
              << "  --llvm                 Generate LLVM IR (default)\n"
              << "  --emit-llvm           Output LLVM IR to file instead of compiling\n"
              << "  --emit-asm            Output assembly code to file instead of compiling\n"
              << "  --debug               Enable debug output (show tokens during lexing)\n"
              << "  -h, --help            Show this help message\n";
}

int main(int argc, char** argv) {
    std::string inputFile;
    std::string outputFile = "a.out";
    int optLevel = 0;
    bool interpreterMode = false;
    bool emitLLVM = false;
    bool emitAsm = false;
    bool debugMode = false;

    static struct option long_options[] = {
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {"optimize", required_argument, 0, 'O'},
        {"interpreter", no_argument, 0, 'I'},
        {"llvm", no_argument, 0, 'L'},
        {"emit-llvm", no_argument, 0, 'E'},
        {"emit-asm", no_argument, 0, 'A'},
        {"debug", no_argument, 0, 'd'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "i:o:O:IEhAd", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'i':
                inputFile = optarg;
                break;
            case 'o':
                outputFile = optarg;
                break;
            case 'O':
                optLevel = std::stoi(optarg);
                if (optLevel < 0 || optLevel > 3) {
                    std::cerr << "Error: Optimization level must be between 0 and 3\n";
                    return 1;
                }
                break;
            case 'I':
                interpreterMode = true;
                break;
            case 'L':
                interpreterMode = false;
                break;
            case 'E':
                emitLLVM = true;
                break;
            case 'A':
                emitAsm = true;
                break;
            case 'd':
                debugMode = true;
                break;
            case 'h':
                printUsage(argv[0]);
                return 0;
            default:
                printUsage(argv[0]);
                return 1;
        }
    }

    if (inputFile.empty()) {
        std::cerr << "Error: Input file is required\n";
        printUsage(argv[0]);
        return 1;
    }

    try {
        // Create temporary directory for intermediate files
        std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "oberon_compiler";
        std::filesystem::create_directories(tempDir);

        // Generate temporary file paths
        std::filesystem::path llFile = tempDir / "output.ll";
        std::filesystem::path objFile = tempDir / "output.o";

        if (!interpreterMode) {
            if (!frontend::compile(inputFile, llFile.string(), debugMode)) {
                std::cerr << "Error: Frontend compilation failed\n";
                return 1;
            }

            if (!middleend::optimize(llFile.string(), optLevel)) {
                std::cerr << "Error: Optimization failed\n";
                return 1;
            }

            if (emitLLVM) {
                // If --emit-llvm is specified, copy the LLVM IR to the output file
                std::filesystem::copy_file(llFile, outputFile, std::filesystem::copy_options::overwrite_existing);
            } else if (emitAsm) {
                // If --emit-asm is specified, generate assembly code
                if (!backend::compile(llFile.string(), outputFile, true)) {
                    std::cerr << "Error: Assembly generation failed\n";
                    return 1;
                }
            } else {
                if (!backend::compile(llFile.string(), objFile.string(), false)) {
                    std::cerr << "Error: Backend compilation failed\n";
                    return 1;
                }
                if (!backend::link(objFile.string(), outputFile)) {
                    std::cerr << "Error: Linking failed\n";
                    return 1;
                }
            }
        }
        if (interpreterMode) {
            if (!frontend::interpret(inputFile, debugMode)) {
                std::cerr << "Error: Interpretation failed\n";
                return 1;
            }
        }
        // Clean up temporary files
        std::filesystem::remove_all(tempDir);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
