 // Genereate C++ code
%skeleton "lalr1.cc"

%code requires {

    #include <vector>

    #include "ast/base.h"
    #include "ast/arithmeticexpression.h"
    #include "ast/booleanexpression.h"
    #include "ast/lvalue.h"
    #include "ast/relationalexpression.h"
    #include "ast/statement.h"

    class FooLexer;
}

%code {
    #include "foolexer.h"
    int yylex(yy::parser::semantic_type* yylval, FooLexer* lexer)
    {
        return lexer->yylex(yylval);
    }
    #define DEBUG_PARSER(X) std::cerr << X; std::cerr.flush();
}

// %token TOK_EOF
%token TOK_SEMICOLON TOK_COLON TOK_COMMA
%token TOK_IF TOK_BEGIN TOK_ELSE TOK_THEN TOK_END TOK_WHILE TOK_TRUE TOK_FALSE TOK_DO
%token TOK_GEQ TOK_LEQ TOK_GT TOK_LT TOK_EQ TOK_NEQ TOK_OR TOK_AMPERSAND TOK_NOT
%token TOK_PARENTHESES_LEFT TOK_PARENTHESES_RIGHT
%token TOK_PLUS TOK_MINUS TOK_MULT TOK_DIV TOK_DIV_INT TOK_DIV_MOD TOK_ASSIGN
%token TOK_VAR TOK_CHAR TOK_INTEGER TOK_REAL

%token TOK_READINT TOK_PRINT TOK_READSTR TOK_READDOUBLE


%token <stringval> TOK_STRING_VALUE
%token <intval> TOK_NUMBER_VALUE
%token <doubleval> TOK_REAL_VALUE
%token <stringval> TOK_IDENTIFIER

%union
{
    std::string* stringval;
    int intval;
    double doubleval;

    Statement* statement;
    StatementBlock* block;
    BooleanExpression* booleanexpression;
    RelationalExpression* relationalexpression;
    ArithmeticExpression* arithmeticexpression;
    LocationValue* lvalue;
}

%parse-param { Statement** result }
%parse-param { FooLexer* lexer }
%lex-param { FooLexer* lexer }

%defines

%type <block> statements
%type <block> statementsI
%type <block> statementsC
%type <statement> statement
%type <statement> statementI
%type <statement> statementC

%type <booleanexpression> booleanexpression
%type <booleanexpression> booleanexpressionO
%type <booleanexpression> booleanexpressionA
%type <booleanexpression> booleanexpressionX
%type <relationalexpression> relationalexpression

%type <arithmeticexpression> arithmeticexpression
%type <arithmeticexpression> arithmeticexpressionA
%type <arithmeticexpression> arithmeticexpressionM
%type <arithmeticexpression> arithmeticexpressionX

%type <lvalue> lvalue

%type <statement> decl_lvalue_int
%type <statement> decl_lvalue_real
%type <statement> decl_lvalue_str

%%

program
    :statements YYEOF
    {
        *result = $statements;
    }
;
statements
    : %empty
    {
        std::vector<Statement*> block;
        $$ = new StatementBlock(block);
    }
    | statementsI
    {
        $$ = $statementsI;
    }
    | statementsC
    {
        $$ = $statementsC;
    }
;
statementsI
    : statementI[s]
    {
        std::vector<Statement*> block;
        block.push_back($s);
        $$ = new StatementBlock(block);
    }
    | statementsI[s] statement[si]
    {
        $s->statements.push_back($si);
        $$ = $s;
    }
;
statementsC
    : statementC[s]
    {
        std::vector<Statement*> block;
        block.push_back($s);
        $$ = new StatementBlock(block);
    }
    | statementsC[s] statementC[sc]
    {
        $s->statements.push_back($sc);
        $$ = $s;
    }
;
statement
    : statementI
    | statementC
;
statementC
    : lvalue TOK_ASSIGN arithmeticexpression TOK_SEMICOLON
    {
        $$ = new StatementAssign($lvalue, $arithmeticexpression);
    }
    | decl_lvalue_int TOK_SEMICOLON
    | decl_lvalue_real TOK_SEMICOLON
    | decl_lvalue_str TOK_SEMICOLON
    | lvalue TOK_ASSIGN TOK_STRING_VALUE TOK_SEMICOLON
    {
        $$ = new StatementAssignStr($lvalue, $TOK_STRING_VALUE);
        delete $TOK_STRING_VALUE;
    }
    | TOK_IF booleanexpression TOK_THEN statementsC[t] TOK_ELSE statementsC[e] TOK_END
    {
        $$ = new StatementIfElseIfElse({$booleanexpression}, {$t}, $e);
    }
    | TOK_WHILE booleanexpression TOK_DO statementsC[s] TOK_END
    {
        $$ = new StatementWhile($booleanexpression, $s);
    }
    | TOK_READINT lvalue TOK_SEMICOLON
    {
        $$ = new StatementReadInt($lvalue);
    }
    | TOK_READDOUBLE lvalue TOK_SEMICOLON
    {
        $$ = new StatementReadDouble($lvalue);
    }
    | TOK_READSTR lvalue TOK_SEMICOLON
    {
        $$ = new StatementRead($lvalue);
    }
    | TOK_PRINT arithmeticexpression TOK_SEMICOLON
    {
        $$ = new StatementWriteVar($arithmeticexpression);
    }
    | TOK_PRINT TOK_STRING_VALUE TOK_SEMICOLON
    {
        $$ = new StatementWrite(*$TOK_STRING_VALUE);
        delete $TOK_STRING_VALUE;
    }
;
statementI
    : TOK_IF booleanexpression TOK_THEN statementsC[t] TOK_END
    {
        $$ = new StatementIfElseIfElse({$booleanexpression}, {$t});
    }
    | TOK_IF booleanexpression TOK_THEN statementsC[t] TOK_ELSE statementsI[e] TOK_END
    {
        $$ = new StatementIfElseIfElse({$booleanexpression}, {$t}, $e);
    }
    | TOK_WHILE booleanexpression TOK_DO statementI[s] TOK_END
    {
        $$ = new StatementWhile($booleanexpression, $s);
    }
;
booleanexpression
    : booleanexpressionO
;
booleanexpressionO
    : booleanexpressionO[l] TOK_OR booleanexpressionA[r]
    {
        $$ = new BooleanExpressionOr($l, $r);
    }
    | booleanexpressionA
;
booleanexpressionA
    : booleanexpressionA[l] TOK_AMPERSAND booleanexpressionX[r]
    {
        $$ = new BooleanExpressionAnd($l, $r);
    }
    | booleanexpressionX
;
booleanexpressionX
    : TOK_TRUE
    {
        $$ = new BooleanExpressionTrue();
    }
    | TOK_FALSE
    {
        $$ = new BooleanExpressionFalse();
    }
    | TOK_PARENTHESES_LEFT booleanexpression[b] TOK_PARENTHESES_RIGHT
    {
        $$ = $b;
    }
    | TOK_NOT booleanexpressionX[b]
    {
        $$ = new BooleanExpressionNot($b);
    }
    | relationalexpression
    {
        $$ = $relationalexpression;
    }
;
relationalexpression
    : arithmeticexpression[l] TOK_GEQ arithmeticexpression[r]
    {
        $$ = new RelationalExpressionGEQ($l, $r);
    }
    | arithmeticexpression[l] TOK_LEQ arithmeticexpression[r]
    {
        $$ = new RelationalExpressionLEQ($l, $r);
    }
    | arithmeticexpression[l] TOK_LT arithmeticexpression[r]
    {
        $$ = new RelationalExpressionLT($l, $r);
    }
    | arithmeticexpression[l] TOK_GT arithmeticexpression[r]
    {
        $$ = new RelationalExpressionGT($l, $r);
    }
    | arithmeticexpression[l] TOK_EQ arithmeticexpression[r]
    {
        $$ = new RelationalExpressionEQ($l, $r);
    }
    | arithmeticexpression[l] TOK_NEQ arithmeticexpression[r]
    {
        $$ = new RelationalExpressionNEQ($l, $r);
    }
;
arithmeticexpression
    : arithmeticexpressionA
;
arithmeticexpressionA
    : arithmeticexpressionA[l] TOK_PLUS arithmeticexpressionM[r]
    {
        $$ = new ArithmeticExpressionPlus($l, $r);
    }
    | arithmeticexpressionA[l] TOK_MINUS arithmeticexpressionM[r]
    {
        $$ = new ArithmeticExpressionMinus($l, $r);
    }
    | arithmeticexpressionM
;

arithmeticexpressionM
    : arithmeticexpressionM[l] TOK_MULT arithmeticexpressionX[r]
    {
        $$ = new ArithmeticExpressionMult($l, $r);
    }
    | arithmeticexpressionM[l] TOK_DIV arithmeticexpressionX[r]
    {
        $$ = new ArithmeticExpressionDiv($l, $r);
    }
    | arithmeticexpressionM[l] TOK_DIV_INT arithmeticexpressionX[r]
    {
        $$ = new ArithmeticExpressionDivInt($l, $r);
    }
    | arithmeticexpressionM[l] TOK_DIV_MOD arithmeticexpressionX[r]
    {
        $$ = new ArithmeticExpressionDivMod($l, $r);
    }
    | arithmeticexpressionX
;
arithmeticexpressionX
    : TOK_NUMBER_VALUE
    {
        $$ = new ArithmeticExpressionConst($TOK_NUMBER_VALUE);
    }
    | TOK_REAL_VALUE
    {
        $$ = new ArithmeticExpressionConst($TOK_REAL_VALUE);
    }
    | lvalue
    {
        $$ = $lvalue;
    }
    | TOK_PARENTHESES_LEFT arithmeticexpression[a] TOK_PARENTHESES_RIGHT
    {
        $$ = $[a];
    }
;
decl_lvalue_int
    : TOK_VAR TOK_IDENTIFIER TOK_COLON TOK_INTEGER
    {
        $$ = new DeclarationStatement<int>(*$TOK_IDENTIFIER);
        delete $TOK_IDENTIFIER;
    }
;
decl_lvalue_real
    : TOK_VAR TOK_IDENTIFIER TOK_COLON TOK_REAL
    {
        $$ = new DeclarationStatement<double>(*$TOK_IDENTIFIER);
        delete $TOK_IDENTIFIER;
    }
;
decl_lvalue_str
    : TOK_VAR TOK_IDENTIFIER TOK_COLON TOK_CHAR
    {
        $$ = new DeclarationStatement<std::string>(*$TOK_IDENTIFIER);
        delete $TOK_IDENTIFIER;
    }
;
lvalue
    : TOK_IDENTIFIER
    {
        $$ = new LocationValueVariable(*$TOK_IDENTIFIER);
        delete $TOK_IDENTIFIER;
    }
%%

void yy::parser::error(const std::string& msg)
{
    std::cerr << msg;
}
