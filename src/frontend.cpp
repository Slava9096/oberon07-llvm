#include <iostream>
#include <FlexLexer.h>

#include "flex/tokens.h"
std::string* pStr = nullptr;

int main(){
    FlexLexer* l = new yyFlexLexer(std::cin, std::cerr);

    while (tokens::token_t token = (tokens::token_t)( l->yylex() )) {
        if(token == tokens::TOK_EOF){
            break;
        }
        switch (token) {
            case tokens::TOK_IF: std::cout << "IF "; break;
            case tokens::TOK_IDENTIFIER: std::cout << "IDENTIFIER(" << *pStr << ") "; delete pStr; pStr = nullptr; break;
            case tokens::TOK_NUMBER: std::cout << "NUMBER(" << *pStr << ") "; delete pStr; pStr = nullptr; break;
            case tokens::TOK_HEXADECIMAL: std::cout << "HEXADECIMAL(" << *pStr << ") "; delete pStr; pStr = nullptr; break;
            case tokens::TOK_REAL: std::cout << "REAL(" << *pStr << ") "; delete pStr; pStr = nullptr; break;
            case tokens::TOK_STRING: std::cout << "STRING(" << *pStr << ") "; delete pStr; pStr = nullptr; break;
            default: std::cout << "Update tokens bud lol"; break;
        }
    }

    delete l;
    return 0;
}
