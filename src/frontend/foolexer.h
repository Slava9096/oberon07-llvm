#ifndef __FOOLEXER_H
#define __FOOLEXER_H

#include <ostream>
#ifndef yyFlexLexerOnce
    #include <FlexLexer.h>
#endif

#include "parser.hpp"

class FooLexer : public yyFlexLexer
{
    private:
        yy::parser::semantic_type* yylval;
        bool debugMode;
        int yylex();
    public:
        FooLexer(std::istream& is, std::ostream& os);
        int yylex(yy::parser::semantic_type* yylval);
        void setDebugMode(bool debug) { debugMode = debug; }
};
#endif
