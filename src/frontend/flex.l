/* definitions */
%{
 #include <iostream>
 #include <string>
 #include <cmath>
 #include "parser.hpp"
 #include "foolexer.h"

 #define DEBUG_LEXER(X) std::cerr << X << " "; std::cerr.flush();
 #define TOKEN(t) DEBUG_LEXER(#t) return yy::parser::token::t

// TODO: Add unicode chars
 std::string interpretEscapes(const std::string& str){
    std::string result;
    for (size_t i = 0; i < str.length(); i++){
        if (str[i] == '\\' && i + 1 < str.length()){
            switch (str[i+1]){
                case 'n':  result += '\n';  i++; break;
                case 't':  result += '\t';  i++; break;
                case 'r':  result += '\r';  i++; break;
                case 'b':  result += '\b';  i++; break;
                case 'f':  result += '\f';  i++; break;
                case '"':  result +=  '"';  i++; break;
                case '\'': result += '\'';  i++; break;
                case '\\': result += '\\';  i++; break;
                case '0':  result += '\0';  i++; break;
                // Unknown escape
                default:
                    result += str[i];
                    result += str[i + 1];
                    i++;
            }
        } else {
            result += str[i];
        }
    }
    return result;
 }
 // Parse scientific notation and convert to double
 double parseDouble(const std::string& number){
     size_t ePos = number.find('E');

     if (ePos == std::string::npos){
         return std::stod(number);
     }

     return std::stod(number.substr(0,ePos)) * std::pow(10, std::stoi(number.substr(ePos + 1)));
 }
%}

/* options */
%option noyywrap
%option c++
%option yyclass="FooLexer"

/* rules */
%%

\(\*.*?\*\)                         { /* ignore comment block */ }
[ \t\n]+                            { /* ignore whitespace */ }

 /* Reserved tokens */

 /* "ARRAY"                             { TOKEN(TOK_ARRAY); } */
"BEGIN"                             { TOKEN(TOK_BEGIN); }
 /* "BY"                                { TOKEN(TOK_BY); } */
 /* "CASE"                              { TOKEN(TOK_CASE); } */
 /* "CONST"                             { TOKEN(TOK_CONST); } */
"CHAR"                              { TOKEN(TOK_CHAR); }
"DIV"                               { TOKEN(TOK_DIV_INT); }
"DO"                                { TOKEN(TOK_DO); }
"ELSE"                              { TOKEN(TOK_ELSE); }
 /* "ELSEIF"                            { TOKEN(TOK_ELSEIF); } */
"END"                               { TOKEN(TOK_END); }
"FALSE"                             { TOKEN(TOK_FALSE); }
 /* "FOR"                               { TOKEN(TOK_FOR); } */
"IF"                                { TOKEN(TOK_IF); }
 /* "IMPORT"                            { TOKEN(TOK_IMPORT); } */
"INTEGER"                           { TOKEN(TOK_INTEGER); }
 /* "IN"                                { TOKEN(TOK_IN); } */
 /* "IS"                                { TOKEN(TOK_IS); } */
"MOD"                               { TOKEN(TOK_DIV_MOD); }
 /* "MODULE"                            { TOKEN(TOK_MODULE); } */
 /* "NIL"                               { TOKEN(TOK_NIL); } */
 /* "OF"                                { TOKEN(TOK_OF); } */
"OR"                                { TOKEN(TOK_OR); }
 /* "POINTER"                           { TOKEN(TOK_POINTER); } */
 /* "PROCEDURE"                         { TOKEN(TOK_PROCEDURE); } */
"REAL"                              { TOKEN(TOK_REAL); }
 /* "RECORD"                            { TOKEN(TOK_RECORD); } */
 /* "REPEAT"                            { TOKEN(TOK_REPEAT); } */
 /* "RETURN"                            { TOKEN(TOK_RETURN); } */
"THEN"                              { TOKEN(TOK_THEN); }
 /* "TO"                                { TOKEN(TOK_TO); } */
"TRUE"                              { TOKEN(TOK_TRUE); }
 /* "TYPE"                              { TOKEN(TOK_TYPE); } */
 /* "UNTIL"                             { TOKEN(TOK_UNTIL); } */
"VAR"                               { TOKEN(TOK_VAR); }
"WHILE"                             { TOKEN(TOK_WHILE); }
 /* TEST: tmp token */
"PRINT"                             { TOKEN(TOK_PRINT); }
"READSTR"                           { TOKEN(TOK_READSTR); }
"READDOUBLE"                        { TOKEN(TOK_READDOUBLE); }
"READINT"                           { TOKEN(TOK_READINT); }
"+"                                 { TOKEN(TOK_PLUS); }
"-"                                 { TOKEN(TOK_MINUS); }
"*"                                 { TOKEN(TOK_MULT); }
"/"                                 { TOKEN(TOK_DIV); }
"~"                                 { TOKEN(TOK_NOT); }
"&"                                 { TOKEN(TOK_AMPERSAND); }
 /* "."                                 { TOKEN(TOK_DOT); } */
","                                 { TOKEN(TOK_COMMA); }
";"                                 { TOKEN(TOK_SEMICOLON); }
":"                                 { TOKEN(TOK_COLON); }
 /* "|"                                 { TOKEN(TOK_BAR); } */
"("                                 { TOKEN(TOK_PARENTHESES_LEFT); }
")"                                 { TOKEN(TOK_PARENTHESES_RIGHT); }
 /* "["                                 { TOKEN(TOK_BRACKETS_LEFT); } */
 /* "]"                                 { TOKEN(TOK_BRACKETS_RIGHT); } */
 /* "{"                                 { TOKEN(TOK_BRACES_LEFT); } */
 /* "}"                                 { TOKEN(TOK_BRACES_RIGHT); } */
":="                                { TOKEN(TOK_ASSIGN); }
 /* "^"                                 { TOKEN(TOK_DEREF); } */
"="                                 { TOKEN(TOK_EQ); }
"#"                                 { TOKEN(TOK_NEQ); }
"<"                                 { TOKEN(TOK_LT); }
">"                                 { TOKEN(TOK_GT); }
"<="                                { TOKEN(TOK_LEQ); }
">="                                { TOKEN(TOK_GEQ); }
 /* ".."                                { TOKEN(TOK_RANGE); } */


[a-zA-Z][a-zA-Z0-9]*                { yylval->stringval = new std::string(yytext, yyleng); TOKEN(TOK_IDENTIFIER); }

 /* Parse string and replace literal escapes with actuall ones */
\"(?:\\\"|[^\"])*\"                 {
                                      std::string raw_str(yytext + 1, yyleng - 2);
                                      std::string str = interpretEscapes(raw_str);
                                      yylval->stringval = new std::string(str);
                                      TOKEN(TOK_STRING_VALUE);
                                    }
 /* [0-9A-F]+X                          { yylval->stringval = new std::string(yytext, yyleng); TOKEN(TOK_STRING_HEX); } */
 /* Parse scientific notation and turn into double */
[0-9]+\.[0-9]+(?:E[\+\-][0-9]+)?    { yylval->doubleval = parseDouble(std::string(yytext, yyleng)); TOKEN(TOK_REAL_VALUE); }
 /* [0-9A-F]+H                          { yylval->stringval = new std::string(yytext, yyleng); TOKEN(TOK_HEXADECIMAL); } */
[0-9]+                              { yylval->intval = stoi(std::string(yytext, yyleng)); TOKEN(TOK_NUMBER_VALUE); }


.                                   { std::cout << "LEXER ERROR: Unknown character";}
<<EOF>>                             { TOKEN(YYEOF); }

%%

FooLexer::FooLexer(std::istream& is, std::ostream& os) : yyFlexLexer(is, os)
{
}
int FooLexer::yylex(yy::parser::semantic_type* yylval)
{
    this->yylval = yylval;
    return yylex();
}
