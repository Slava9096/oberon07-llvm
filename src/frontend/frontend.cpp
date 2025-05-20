#include <iostream>
#include <fstream>
#include <FlexLexer.h>

#include "flex/tokens.h"
std::string* pStr = nullptr;

int main(int argc, char* argv[]){

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::ifstream program(argv[1]);
    if (!program) {
        std::cerr << "Could not open file: " << argv[1] << std::endl;
        return 1;
    }

    FlexLexer* lexer = new yyFlexLexer(program, std::cerr);

    delete lexer;
    return 0;
}
