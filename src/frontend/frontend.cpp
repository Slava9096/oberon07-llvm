#include <fstream>

#include "foolexer.h"

#include "ast/base.h"
#include "parser.hpp"

int main(int argc, char** argv){

    if(argc < 2) return 1;
    std::ifstream src(argv[1]);

    Statement* program = nullptr;
    FooLexer* lexer = new FooLexer(src, std::cerr);
    yy::parser* parser = new yy::parser(&program, lexer);
    if(parser->parse() == 0)
    {
        Context* context = new Context();
        program->Execute(context);
        delete context;
    }
    delete program;

    delete parser;
    delete lexer;
    return 0;
}
