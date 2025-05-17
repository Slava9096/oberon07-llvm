/* definitions */
%{
 #include <string>
 #include "tokens.h"
 #define SAVE_TOKEN pStr = new std::string(yytext, yyleng)
 #define TOKEN(t) return tokens::t
 extern std::string* pStr;
%}

/* options */
%option noyywrap
%option c++

/* rules */
%%

\(\*.*?\*\)                         { /* ignore comment block */ }
[ \t\n]+                            { /* ignore whitespace */ }

 /* Reserved tokens */

"NIL"                               { TOKEN(TOK_NIL); }
"TRUE"                              { TOKEN(TOK_TRUE); }
"FALSE"                             { TOKEN(TOK_FALSE); }
"ARRAY"                             { TOKEN(TOK_ARRAY); }
"RECORD"                            { TOKEN(TOK_RECORD); }
"POINTER"                           { TOKEN(TOK_POINTER); }
"TO"                                { TOKEN(TOK_TO); }
"PROCEDURE"                         { TOKEN(TOK_PROCEDURE); }
"MODULE"                            { TOKEN(TOK_MODULE); }
"VAR"                               { TOKEN(TOK_VAR); }
"TYPE"                              { TOKEN(TOK_TYPE); }
"IMPORT"                            { TOKEN(TOK_IMPORT); }
"CONST"                             { TOKEN(TOK_CONST); }
"RETURN"                            { TOKEN(TOK_RETURN); }
"OF"                                { TOKEN(TOK_OF); }
"UNTIL"                             { TOKEN(TOK_UNTIL); }
"BEGIN"                             { TOKEN(TOK_BEGIN); }
"END"                               { TOKEN(TOK_END); }
"IF"                                { TOKEN(TOK_IF); }
"THEN"                              { TOKEN(TOK_THEN); }
"ELSEIF"                            { TOKEN(TOK_ELSEIF); }
"ELSE"                              { TOKEN(TOK_ELSE); }
"CASE"                              { TOKEN(TOK_CASE); }
"WHILE"                             { TOKEN(TOK_WHILE); }
"DO"                                { TOKEN(TOK_DO); }
"REPEAT"                            { TOKEN(TOK_REPEAT); }
"FOR"                               { TOKEN(TOK_FOR); }


[a-zA-Z][a-zA-Z0-9]*                { SAVE_TOKEN; TOKEN(TOK_IDENTIFIER); }

\"(?:\\\"|[^\"])*\"                 { SAVE_TOKEN; TOKEN(TOK_STRING); }
[0-9A-F]+X                          { SAVE_TOKEN; TOKEN(TOK_STRING_HEX); }

[0-9]+\.[0-9]+(?:E[\+\-][0-9]+)?    { SAVE_TOKEN; TOKEN(TOK_REAL); }
[0-9A-F]+H                          { SAVE_TOKEN; TOKEN(TOK_HEXADECIMAL); }
[0-9]+                              { SAVE_TOKEN; TOKEN(TOK_NUMBER); }


.                                   { std::cout << "LEXICAL ERROR: Invalid token\n";}
<<EOF>>                             { TOKEN(TOK_EOF); }

%%
