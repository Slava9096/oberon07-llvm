 // Genereate C++ code
%skeleton "lalr1.cc"

%code requires {

    #include <vector>

    #include "base.h"
    #include "arithmeticexpression.h"
    #include "booleanexpression.h"
    #include "lvalue.h"
    #include "relationalexpression.h"
    #include "statement.h"

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
%token TOK_SEMICOLON
%token TOK_READ TOK_PRINT
%token TOK_IF TOK_BEGIN TOK_ELSE TOK_THEN TOK_END TOK_WHILE TOK_TRUE TOK_FALSE TOK_DO
%token TOK_GEQ TOK_LEQ TOK_GT TOK_LT TOK_EQ TOK_NEQ TOK_OR TOK_AMPERSAND TOK_NOT
%token TOK_PARENTHESES_LEFT TOK_PARENTHESES_RIGHT
%token TOK_PLUS TOK_MINUS TOK_MULT TOK_DIV TOK_DIV_INT TOK_DIV_MOD TOK_ASSIGN


%token <stringval> TOK_STRING
%token <intval> TOK_NUMBER
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
    ArithmeticExpression<int>* arithmeticexpression;
    LocationValue<int>* lvalue;
}

%parse-param { Statement** result }
%parse-param { FooLexer* lexer }
%lex-param { FooLexer* lexer }

%defines

%type <block> statements
%type <statement> statement

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
    | statements[s] statement
    {
        $s->statements.push_back($statement);
        $$ = $s;
    }
;
statement
    : lvalue TOK_ASSIGN arithmeticexpression TOK_SEMICOLON
    {
        $$ = new StatementAssign($lvalue, $arithmeticexpression);
    }
    | TOK_IF booleanexpression TOK_THEN statement[t] TOK_END
    {
        $$ = new StatementIfElseIfElse({$booleanexpression}, {$t});
    }
    | TOK_IF booleanexpression TOK_THEN statement[t] TOK_ELSE statement[e] TOK_END
    {
        $$ = new StatementIfElseIfElse({$booleanexpression}, {$t}, $e);
    }
    | TOK_WHILE booleanexpression TOK_DO statement[s]
    {
        $$ = new StatementWhile($booleanexpression, $s);
    }
    | TOK_READ lvalue TOK_SEMICOLON
    {
        $$ = new StatementReadInt($lvalue);
    }
    | TOK_PRINT arithmeticexpression TOK_SEMICOLON
    {
        $$ = new StatementWriteInt($arithmeticexpression);
    }
    | TOK_PRINT TOK_STRING TOK_SEMICOLON
    {
        $$ = new StatementWrite(*$TOK_STRING);
        delete $TOK_STRING;
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
        $$ = new ArithmeticExpressionPlus<int>($l, $r);
    }
    | arithmeticexpressionA[l] TOK_MINUS arithmeticexpressionM[r]
    {
        $$ = new ArithmeticExpressionMinus<int>($l, $r);
    }
    | arithmeticexpressionM
;

arithmeticexpressionM
    : arithmeticexpressionM[l] TOK_MULT arithmeticexpressionX[r]
    {
        $$ = new ArithmeticExpressionMult<int>($l, $r);
    }
    | arithmeticexpressionM[l] TOK_DIV arithmeticexpressionX[r]
    {
        $$ = new ArithmeticExpressionDiv<int>($l, $r);
    }
    | arithmeticexpressionM[l] TOK_DIV_INT arithmeticexpressionX[r]
    {
        $$ = new ArithmeticExpressionDivInt<int>($l, $r);
    }
    | arithmeticexpressionM[l] TOK_DIV_MOD arithmeticexpressionX[r]
    {
        $$ = new ArithmeticExpressionDivMod<int>($l, $r);
    }
    | arithmeticexpressionX
;
arithmeticexpressionX
    : TOK_NUMBER
    {
        $$ = new ArithmeticExpressionConst<int>($TOK_NUMBER);
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
lvalue
    : TOK_IDENTIFIER
    {
        $$ = new LocationValueVariable<int>(*$TOK_IDENTIFIER);
        delete $TOK_IDENTIFIER;
    }
;
%%

void yy::parser::error(const std::string& msg)
{
    std::cerr << msg;
}
