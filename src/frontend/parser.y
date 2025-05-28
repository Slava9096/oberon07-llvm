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
    ArithmeticExpression<int>* arithmeticexpression_int;
    ArithmeticExpression<double>* arithmeticexpression_real;
    LocationValue<int>* lvalue_int;
    LocationValue<double>* lvalue_real;
    LocationValue<std::string>* lvalue_str;
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

%type <arithmeticexpression_int> arithmeticexpression_int
%type <arithmeticexpression_int> arithmeticexpressionA_int
%type <arithmeticexpression_int> arithmeticexpressionM_int
%type <arithmeticexpression_int> arithmeticexpressionX_int

%type <arithmeticexpression_real> arithmeticexpression_real
%type <arithmeticexpression_real> arithmeticexpressionA_real
%type <arithmeticexpression_real> arithmeticexpressionM_real
%type <arithmeticexpression_real> arithmeticexpressionX_real

%type <lvalue_int> lvalue_int
%type <lvalue_real> lvalue_real
%type <lvalue_str> lvalue_str

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
    : lvalue_int[lvalue] TOK_ASSIGN arithmeticexpression_int TOK_SEMICOLON
    {
        $$ = new StatementAssign($lvalue, $arithmeticexpression_int);
    }
    | lvalue_real[lvalue] TOK_ASSIGN arithmeticexpression_real TOK_SEMICOLON
    {
        $$ = new StatementAssign($lvalue, $arithmeticexpression_real);
    }
    | lvalue_str[lvalue] TOK_ASSIGN TOK_STRING_VALUE TOK_SEMICOLON
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
    | TOK_READINT lvalue_int[lvalue] TOK_SEMICOLON
    {
        $$ = new StatementReadInt($lvalue);
    }
    | TOK_READDOUBLE lvalue_real[lvalue] TOK_SEMICOLON
    {
        $$ = new StatementReadDouble($lvalue);
    }
    | TOK_READSTR lvalue_str[lvalue] TOK_SEMICOLON
    {
        $$ = new StatementRead($lvalue);
    }
    | TOK_PRINT arithmeticexpression_int TOK_SEMICOLON
    {
        $$ = new StatementWriteInt($arithmeticexpression_int);
    }
    | TOK_PRINT arithmeticexpression_real TOK_SEMICOLON
    {
        $$ = new StatementWriteDouble($arithmeticexpression_real);
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
    : arithmeticexpression_int[l] TOK_GEQ arithmeticexpression_int[r]
    {
        $$ = new RelationalExpressionGEQ($l, $r);
    }
    | arithmeticexpression_int[l] TOK_LEQ arithmeticexpression_int[r]
    {
        $$ = new RelationalExpressionLEQ($l, $r);
    }
    | arithmeticexpression_int[l] TOK_LT arithmeticexpression_int[r]
    {
        $$ = new RelationalExpressionLT($l, $r);
    }
    | arithmeticexpression_int[l] TOK_GT arithmeticexpression_int[r]
    {
        $$ = new RelationalExpressionGT($l, $r);
    }
    | arithmeticexpression_int[l] TOK_EQ arithmeticexpression_int[r]
    {
        $$ = new RelationalExpressionEQ($l, $r);
    }
    | arithmeticexpression_int[l] TOK_NEQ arithmeticexpression_int[r]
    {
        $$ = new RelationalExpressionNEQ($l, $r);
    }
    | arithmeticexpression_real[l] TOK_GEQ arithmeticexpression_real[r]
    {
        $$ = new RelationalExpressionGEQ($l, $r);
    }
    | arithmeticexpression_real[l] TOK_LEQ arithmeticexpression_real[r]
    {
        $$ = new RelationalExpressionLEQ($l, $r);
    }
    | arithmeticexpression_real[l] TOK_LT arithmeticexpression_real[r]
    {
        $$ = new RelationalExpressionLT($l, $r);
    }
    | arithmeticexpression_real[l] TOK_GT arithmeticexpression_real[r]
    {
        $$ = new RelationalExpressionGT($l, $r);
    }
    | arithmeticexpression_real[l] TOK_EQ arithmeticexpression_real[r]
    {
        $$ = new RelationalExpressionEQ($l, $r);
    }
    | arithmeticexpression_real[l] TOK_NEQ arithmeticexpression_real[r]
    {
        $$ = new RelationalExpressionNEQ($l, $r);
    }
;
arithmeticexpression_int
    : arithmeticexpressionA_int
;
arithmeticexpressionA_int
    : arithmeticexpressionA_int[l] TOK_PLUS arithmeticexpressionM_int[r]
    {
        $$ = new ArithmeticExpressionPlus<int>($l, $r);
    }
    | arithmeticexpressionA_int[l] TOK_MINUS arithmeticexpressionM_int[r]
    {
        $$ = new ArithmeticExpressionMinus<int>($l, $r);
    }
    | arithmeticexpressionM_int
;

arithmeticexpressionM_int
    : arithmeticexpressionM_int[l] TOK_MULT arithmeticexpressionX_int[r]
    {
        $$ = new ArithmeticExpressionMult<int>($l, $r);
    }
    | arithmeticexpressionM_int[l] TOK_DIV arithmeticexpressionX_int[r]
    {
        $$ = new ArithmeticExpressionDiv<int>($l, $r);
    }
    | arithmeticexpressionM_int[l] TOK_DIV_INT arithmeticexpressionX_int[r]
    {
        $$ = new ArithmeticExpressionDivInt<int>($l, $r);
    }
    | arithmeticexpressionM_int[l] TOK_DIV_MOD arithmeticexpressionX_int[r]
    {
        $$ = new ArithmeticExpressionDivMod<int>($l, $r);
    }
    | arithmeticexpressionX_int
;
arithmeticexpressionX_int
    : TOK_NUMBER_VALUE
    {
        $$ = new ArithmeticExpressionConst<int>($TOK_NUMBER_VALUE);
    }
    | lvalue_int[lvalue]
    {
        $$ = $lvalue;
    }
    | TOK_PARENTHESES_LEFT arithmeticexpression_int[a] TOK_PARENTHESES_RIGHT
    {
        $$ = $[a];
    }
;
arithmeticexpression_real
    : arithmeticexpressionA_real
;
arithmeticexpressionA_real
    : arithmeticexpressionA_real[l] TOK_PLUS arithmeticexpressionM_real[r]
    {
        $$ = new ArithmeticExpressionPlus<double>($l, $r);
    }
    | arithmeticexpressionA_real[l] TOK_MINUS arithmeticexpressionM_real[r]
    {
        $$ = new ArithmeticExpressionMinus<double>($l, $r);
    }
    | arithmeticexpressionM_real
;

arithmeticexpressionM_real
    : arithmeticexpressionM_real[l] TOK_MULT arithmeticexpressionX_real[r]
    {
        $$ = new ArithmeticExpressionMult<double>($l, $r);
    }
    | arithmeticexpressionM_real[l] TOK_DIV arithmeticexpressionX_real[r]
    {
        $$ = new ArithmeticExpressionDiv<double>($l, $r);
    }
    | arithmeticexpressionX_real
;
arithmeticexpressionX_real
    : TOK_REAL_VALUE
    {
        $$ = new ArithmeticExpressionConst<double>($TOK_REAL_VALUE);
    }
    | lvalue_real[lvalue]
    {
        $$ = $lvalue;
    }
    | TOK_PARENTHESES_LEFT arithmeticexpression_real[a] TOK_PARENTHESES_RIGHT
    {
        $$ = $[a];
    }
;
lvalue_int
    : TOK_VAR TOK_IDENTIFIER TOK_COLON TOK_INTEGER
    {
        $$ = new LocationValueVariable<int>(*$TOK_IDENTIFIER);
        delete $TOK_IDENTIFIER;
    }
;
lvalue_real
    : TOK_VAR TOK_IDENTIFIER TOK_COLON TOK_REAL
    {
        $$ = new LocationValueVariable<double>(*$TOK_IDENTIFIER);
        delete $TOK_IDENTIFIER;
    }
;
lvalue_str
    : TOK_VAR TOK_IDENTIFIER TOK_COLON TOK_CHAR
    {
        $$ = new LocationValueVariable<std::string>(*$TOK_IDENTIFIER);
        delete $TOK_IDENTIFIER;
    }
;
%%

void yy::parser::error(const std::string& msg)
{
    std::cerr << msg;
}
