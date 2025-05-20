%{
/* definitions */
#include <iostream>
#include <string>

%}

%token TOK_EOF TOK_ERROR
%token TOK_FALSE TOK_TRUE TOK_NIL
%token TOK_IF TOK_ELSE TOK_ELSEIF TOK_WHILE TOK_REPEAT TOK_FOR TOK_DO TOK_CASE TOK_OF TOK_BEGIN TOK_END TOK_RETURN TOK_IMPORT TOK_UNTIL TOK_THEN TOK_TO TOK_BY TOK_IN TOK_IS TOK_MODULE TOK_VAR TOK_CONST TOK_PROCEDURE TOK_TYPE 
%token TOK_ASSIGN TOK_EQ TOK_NEQ TOK_LEQ TOK_LESS TOK_GRQ TOK_GREATER TOK_PLUS TOK_MINUS TOK_MULT TOK_DIV TOK_DIV_INT TOK_DIV_MOD TOK_NOT TOK_OR TOK_AND TOK_AMPERSAND TOK_DOT TOK_COMMA TOK_COLON TOK_SEMICOLON TOK_PARENTHESES_LEFT TOK_PARENTHESES_RIGHT TOK_BRACES_LEFT TOK_BRACES_RIGHT TOK_BRACKETS_LEFT TOK_BRACKETS_RIGHT TOK_DEREF TOK_BAR TOK_RANGE
%token TOK_BOOLEAN TOK_INTEGER TOK_CHAR TOK_REAL TOK_POINTER TOK_RECORD TOK_ARRAY TOK_OF TOK_STRING TOK_REAL TOK_NUMBER TOK_IDENTIFIER TOK_STRING_HEX TOK_HEXADECIMAL

%language "c++"
%defines
%define api.value.type union
%union {
    int token;
    std::string* str_val;
}
/* rules */
%%

number
    :
;
string
    :
;
integer
    :
;
real
    :
;
scale_factor
    :
;

programm
    : module TOK_EOF
;
module
    : TOK_MODULE ident TOK_SEMICOLON declaration_sequence TOK_END TOK_DOT
    | TOK_MODULE ident TOK_SEMICOLON imports declaration_sequence TOK_END TOK_DOT
    | TOK_MODULE ident TOK_SEMICOLON declaration_sequence TOK_BEGIN statements TOK_END TOK_DOT
    | TOK_MODULE ident TOK_SEMICOLON imports declaration_sequence TOK_BEGIN statements TOK_END TOK_DOT
;
imports
    : TOK_IMPORT import_list TOK_SEMICOLON
;
import_list
    : import
    | import_list TOK_COMMA import
;
import
    : ident
    | ident TOK_ASSIGN ident
;
ident
    : TOK_IDENTIFIER
;
identdef
    : ident
    | ident TOK_MULT
;
qualident
    : ident TOK_DOT ident
    | ident
;
ident_list
    : ident
    | ident_list TOK_COMMA ident
;
identdef_list
    : identdef
    | identdef_list TOK_COMMA identdef
;
expression
    : simple_expression
    | simple_expression relation simple_expression
;
relation
    : TOK_EQ
    | TOK_NEQ
    | TOK_LESS
    | TOK_LEQ
    | TOK_GREATER
    | TOK_GRQ
    | TOK_IN
    | TOK_IS
;
simple_expression
    : term
    | TOK_PLUS term
    | TOK_MINUS term
    | TOK_PLUS term add_operators
    | TOK_MINUS term add_operators
;
add_operators
    : add_operator term
    | add_operators add_operator term
;
add_operator
    : TOK_PLUS
    | TOK_MINUS
    | TOK_OR
;
term
    : factor mul_operators
;
mul_operators
    : TOK_MULT
    | TOK_DIV
    | TOK_DIV_INT
    | TOK_DIV_MOD
    | TOK_AMPERSAND
;
// TODO:
factor
    : number
    | string
    | TOK_NIL
    | TOK_TRUE
    | TOK_FALSE
    | set
    | designator
    | designator actual_parameters
    | TOK_PARENTHESES_LEFT qualident TOK_PARENTHESES_RIGHT
    | TOK_NOT factor
;
actual_parameters
    : TOK_PARENTHESES_LEFT TOK_PARENTHESES_RIGHT
    | TOK_PARENTHESES_LEFT exp_list TOK_PARENTHESES_RIGHT
set
    : TOK_BRACES_LEFT TOK_BRACES_RIGHT
    | TOK_BRACES_LEFT elements TOK_BRACES_RIGHT
;
elements
    : element
    | elements TOK_COMMA element
;
element
    : expression
    | expression TOK_RANGE expression
;
declaration_sequence
    : %empty
    | const_section
    | const_section procedure_section
    | const_section type_section
    | const_section type_section procedure_section
    | const_section type_section var_section
    | const_section type_section var_section procedure_section
    | const_section var_section
    | const_section var_section procedure_section
    | procedure_section
    | type_section
    | type_section procedure_section
    | type_section var_section
    | type_section var_section procedure_section
    | var_section
    | var_section procedure_section
;
const_section
    : TOK_CONST define_consts TOK_SEMICOLON
;
define_consts
    : const_declaration
    | define_consts const_declaration
;
const_declaration
    : identdef TOK_EQ const_expression
;
const_expression
    : expression
;
type_section
    : type_declaration
    | type_section type_declaration
;
type_declaration
    : identdef TOK_EQ type
;
var_section
    : var_declaration
    | var_section var_declaration
;
var_declaration
    : identdef_list TOK_COLON type
;
procedure_section
    : procedure_declaration
    | procedure_section procedure_declaration
;
procedure_declaration
    : procedure_heading TOK_SEMICOLON procedure_body ident
;
procedure_heading
    : TOK_PROCEDURE identdef formal_parameters
    | TOK_PROCEDURE identdef
;
procedure_body
    : declaration_sequence TOK_END
    | declaration_sequence TOK_BEGIN statements TOK_END
    | declaration_sequence TOK_BEGIN statements TOK_RETURN expression TOK_END
    | declaration_sequence TOK_RETURN expression TOK_END
;
formal_parameters
    : TOK_PARENTHESES_LEFT TOK_PARENTHESES_RIGHT
    | TOK_PARENTHESES_LEFT fp_sections TOK_PARENTHESES_RIGHT
    | TOK_PARENTHESES_LEFT fp_sections TOK_PARENTHESES_RIGHT TOK_COLON qualident
    | TOK_PARENTHESES_LEFT TOK_PARENTHESES_RIGHT TOK_COLON qualident
;
fp_sections
    : fp_section
    | fp_sections fp_section
;
fp_section
    : ident TOK_COLON formal_type
    | TOK_VAR ident TOK_COLON formal_type
    | TOK_VAR ident_list TOK_COLON formal_type
;
// WARN: if types doesnt work problem could be here
type
    : qualident
    | TOK_ARRAY
    | TOK_RECORD
    | TOK_POINTER
    | TOK_PROCEDURE
;
formal_type
    : qualident
    | TOK_ARRAY TOK_OF formal_type
;
statements
    : %empty
    | statements TOK_SEMICOLON statement
;
statement
    : assignment
    | procedure_Call
    | if_statement
    | case_statement
    | while_statement
    | repeat_statement
    | for_statement
;
assignment
    : designator TOK_ASSIGN expression
;
designator
    : qualident selectors
;
selectors
    : selector
    | selectors selector
;
selector
    : TOK_DOT ident
    | TOK_BRACKETS_LEFT exp_list TOK_BRACKETS_RIGHT
    | TOK_DEREF
    | TOK_PARENTHESES_LEFT qualident TOK_PARENTHESES_RIGHT
;
exp_list
    : expression
    | exp_list TOX_COMMA expression
;
procedure_call
    : designator
    | designator actual_parameters
;
if_statement
    : TOK_IF expression TOK_THEN statements TOK_END
    | TOK_IF expression TOK_THEN statements TOK_ELSE statements TOK_END
    | TOK_IF expression TOK_THEN statements elseif_blocks TOK_END
;
elseif_blocks
    : TOK_ELSEIF expression TOK_THEN statements
    | elseif_blocks TOK_ELSEIF expression TOK_THEN statements
;
case_statement
    : TOK_CASE expression TOK_OF cases TOK_END
;
cases
    : case
    | cases TOK_BAR case
;
case
    : %empty
    | case_label_list TOK_COLON statements
;
case_label_list
    : label_range
    | case_label_list TOK_COMMA label_range
;
label_range
    : label
    | label TOK_RANGE label
;
// TODO:
label
    : integer
    | string
    | qualident
;
while_statement
    : TOK_WHILE expression TOK_DO statements TOK_END
    | TOK_WHILE expression TOK_DO statements while_elseif_blocks TOK_END
;
while_elseif_blocks
    : TOK_ELSEIF expression TOK_DO statements
    | while_elseif_blocks TOK_ELSEIF expression TOK_DO statements
;
repeat_statement
    : TOK_REPEAT statements TOK_UNTIL expression
;
for_statement
    : TOK_FOR ident TOK_ASSIGN expression TOK_TO expression TOK_DO statements TOK_END
    | TOK_FOR ident TOK_ASSIGN expression TOK_TO expression TOK_BY const_expression TOK_DO statements TOK_END
;

%%
