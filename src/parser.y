// References: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html

%code requires{
  #include "ast.hpp"

  #include <cassert>
  #include <string>

  extern FILE *yyin;
  extern const Node *g_root;

  // Declare functions provided by Flex,
  // so that Bison generated code can call them.
  int yylex(void);
  void yyerror(const char *);
}

%union{
    const Node *node;
    double number;
    std::string *string;
}

%token IDENTIFIER CONSTANT STRING_LITERAL CHAR_LITERAL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN TYPE_NAME

%token TYPEDEF EXTERN STATIC AUTO REGISTER
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN


%type <number> CONSTANT
%type <string> IDENTIFIER STRING_LITERAL CHAR_LITERAL

%type <node> primary_expression postfix_expression argument_expression_list
%type <node> unary_expression unary_operator cast_expression
%type <node> multiplicative_expression additive_expression shift_expression
%type <node> relational_expression equality_expression and_expression
%type <node> exclusive_or_expression inclusive_or_expression
%type <node> logical_and_expression logical_or_expression
%type <node> conditional_expression assignment_expression assignment_operator
%type <node> expression constant_expression
%type <node> declaration declaration_specifiers
%type <node> init_declarator_list init_declarator
%type <node> storage_class_specifier type_specifier
%type <node> struct_or_union_specifier struct_or_union
%type <node> struct_declarator_list struct_declarator
%type <node> enum_specifier enumerator_list enumerator
%type <node> type_qualifier declarator direct_declarator pointer
%type <node> type_qualifier_list parameter_type_list parameter_list
%type <node> parameter_declaration identifier_list type_name
%type <node> abstract_declarator direct_abstract_declarator
%type <node> initializer initializer_list
%type <node> statement labeled_statement compound_statement
%type <node> declaration_list statement_list
%type <node> expression_statement selection_statement iteration_statement
%type <node> jump_statement translation_unit
%type <node> external_declaration function_definition

%start root
%%

root
    : translation_unit                                      { g_root = $1; }

primary_expression
    : IDENTIFIER                                            { $$ = new Identifier(*$1); }
    | CONSTANT                                              { $$ = new Number($1); }
    | STRING_LITERAL
    | '(' expression ')'                                    { $$ = $2; }
    ;

postfix_expression
    : primary_expression                                    { $$ = $1; }
    | postfix_expression '[' expression ']'
    | postfix_expression '(' ')'                            { $$ = new FunctionCall($1, NULL); }
    | postfix_expression '(' argument_expression_list ')'   { $$ = new FunctionCall($1, $3); }
    | postfix_expression '.' IDENTIFIER
    | postfix_expression PTR_OP IDENTIFIER
    | postfix_expression INC_OP                             { $$ = new PostIncrement($1); }
    | postfix_expression DEC_OP
    ;

argument_expression_list
    : assignment_expression                                 { $$ = $1; }
    | argument_expression_list ',' assignment_expression
        { $$ = new FunctionParameterList($1, $3); }
    ;

unary_expression
    : postfix_expression                                    { $$ = $1; }
    | INC_OP unary_expression                               { $$ = new PreIncrement($2); }
    | DEC_OP unary_expression
    | unary_operator cast_expression
    | SIZEOF unary_expression
    | SIZEOF '(' type_name ')'
    ;

unary_operator
    : '&'
    | '*'
    | '+'
    | '-'
    | '~'
    | '!'
    ;

cast_expression
    : unary_expression                                      { $$ = $1; }
    | '(' type_name ')' cast_expression
    ;

multiplicative_expression
    : cast_expression                                       { $$ = $1; }
    | multiplicative_expression '*' cast_expression
        { $$ = new Mul($1, $3); }
    | multiplicative_expression '/' cast_expression
        { $$ = new Divide($1, $3); }
    | multiplicative_expression '%' cast_expression
    ;

additive_expression
    : multiplicative_expression                             { $$ = $1; }
    | additive_expression '+' multiplicative_expression
        { $$ = new Add($1, $3); }
    | additive_expression '-' multiplicative_expression
        { $$ = new Sub($1, $3); }
    ;

shift_expression
    : additive_expression                                   { $$ = $1; }
    | shift_expression LEFT_OP additive_expression
    | shift_expression RIGHT_OP additive_expression
    ;

relational_expression
    : shift_expression                                      { $$ = $1; }
    | relational_expression '<' shift_expression
        { $$ = new LessThan($1, $3);}
    | relational_expression '>' shift_expression
    | relational_expression LE_OP shift_expression
        { $$ = new LessThanEqual($1, $3);}
    | relational_expression GE_OP shift_expression
    ;

equality_expression
    : relational_expression                                 { $$ = $1; }
    | equality_expression EQ_OP relational_expression
        { $$ = new Equal($1, $3); }
    | equality_expression NE_OP relational_expression
    ;

and_expression
    : equality_expression                                   { $$ = $1; }
    | and_expression '&' equality_expression
        { $$ = new BitwiseAnd($1, $3); }
    ;

exclusive_or_expression
    : and_expression                                        { $$ = $1; }
    | exclusive_or_expression '^' and_expression
        { $$ = new BitwiseXor($1, $3);}
    ;

inclusive_or_expression
    : exclusive_or_expression                               { $$ = $1; }
    | inclusive_or_expression '|' exclusive_or_expression
        { $$ = new BitwiseOr($1, $3); }
    ;

logical_and_expression
    : inclusive_or_expression                               { $$ = $1; }
    | logical_and_expression AND_OP inclusive_or_expression
        { $$ = new LogicalAnd($1, $3);}
    ;

logical_or_expression
    : logical_and_expression                                { $$ = $1; }
    | logical_or_expression OR_OP logical_and_expression
        { $$ = new LogicalOr($1, $3); }
    ;

conditional_expression
    : logical_or_expression                                 { $$ = $1; }
    | logical_or_expression '?' expression ':' conditional_expression
        /* { $$ = new Ternary($3, $5, $7); } */
    ;

assignment_expression
    : conditional_expression                                { $$ = $1; }
    | unary_expression assignment_operator assignment_expression
        { $$ = new Assign($1, $2, $3); }
    ;

assignment_operator
    : '='                   { $$ = new AssignOp("="); }
    | MUL_ASSIGN            { $$ = new AssignOp("*="); }
    | DIV_ASSIGN            { $$ = new AssignOp("/="); }
    | MOD_ASSIGN            { $$ = new AssignOp("%="); }
    | ADD_ASSIGN            { $$ = new AssignOp("+="); }
    | SUB_ASSIGN            { $$ = new AssignOp("-="); }
    | LEFT_ASSIGN           { $$ = new AssignOp("<<="); }
    | RIGHT_ASSIGN          { $$ = new AssignOp(">>="); }
    | AND_ASSIGN            { $$ = new AssignOp("&="); }
    | XOR_ASSIGN            { $$ = new AssignOp("~="); }
    | OR_ASSIGN             { $$ = new AssignOp("|="); }
    ;

expression
    : assignment_expression                                 { $$ = $1; }
    | expression ',' assignment_expression
    ;

constant_expression
    : conditional_expression
    ;

declaration
    : declaration_specifiers ';'                            { $$ = $1; }
    | declaration_specifiers init_declarator_list ';'       { $$ = new VariableDeclaration($1, $2); }
    ;

declaration_specifiers
    : storage_class_specifier                               { $$ = $1; }
    | storage_class_specifier declaration_specifiers
    | type_specifier                                        { $$ = $1; }
    | type_specifier declaration_specifiers
    | type_qualifier                                        { $$ = $1; }
    | type_qualifier declaration_specifiers
    ;

init_declarator_list
    : init_declarator                                       { $$ = $1; }
    | init_declarator_list ',' init_declarator
    ;

init_declarator
    : declarator                                            { $$ = $1; }
    | declarator '=' initializer                            { $$ = new Assign($1, new AssignOp("="), $3); }
    ;

storage_class_specifier
    : TYPEDEF
    | EXTERN
    | STATIC
    | AUTO
    | REGISTER
    ;

type_specifier
    : VOID                                                  { $$ = new Type(Types::VOID); }
    | CHAR                                                  { $$ = new Type(Types::CHAR); }
    | SHORT                                                 { $$ = new Type(Types::SHORT); }
    | INT                                                   { $$ = new Type(Types::INT); }
    | LONG                                                  { $$ = new Type(Types::LONG); }
    | FLOAT                                                 { $$ = new Type(Types::FLOAT); }
    | DOUBLE                                                { $$ = new Type(Types::DOUBLE); }
    /* NOTE: This is a temporary solution- ignore compound types. */
    | SIGNED                                                { $$ = new Type(Types::INT); }
    | UNSIGNED                                              { $$ = new Type(Types::UNSIGNED_INT); }
    | struct_or_union_specifier
    | enum_specifier
    | TYPE_NAME
    ;

struct_or_union_specifier
    : struct_or_union IDENTIFIER '{' struct_declaration_list '}'
    | struct_or_union '{' struct_declaration_list '}'
    | struct_or_union IDENTIFIER
    ;

struct_or_union
    : STRUCT
    | UNION
    ;

struct_declaration_list
    : struct_declaration
    | struct_declaration_list struct_declaration
    ;

struct_declaration
    : specifier_qualifier_list struct_declarator_list ';'
    ;

specifier_qualifier_list
    : type_specifier specifier_qualifier_list
    | type_specifier
    | type_qualifier specifier_qualifier_list
    | type_qualifier
    ;

struct_declarator_list
    : struct_declarator
    | struct_declarator_list ',' struct_declarator
    ;

struct_declarator
    : declarator
    | ':' constant_expression
    | declarator ':' constant_expression
    ;

enum_specifier
    : ENUM '{' enumerator_list '}'
    | ENUM IDENTIFIER '{' enumerator_list '}'
    | ENUM IDENTIFIER
    ;

enumerator_list
    : enumerator
    | enumerator_list ',' enumerator
    ;

enumerator
    : IDENTIFIER
    | IDENTIFIER '=' constant_expression
    ;

type_qualifier
    : CONST
    | VOLATILE
    ;

declarator
    : pointer direct_declarator
    | direct_declarator                                     { $$ = $1; }
    ;

direct_declarator
    : IDENTIFIER                                            { $$ = new Identifier(*$1); }
    | '(' declarator ')'                                    { $$ = $2; }
    /* Array declarations with size or without size: arr[5] or arr[] */
    | direct_declarator '[' constant_expression ']'
    | direct_declarator '[' ']'
    /* Function declarators like so: f(int x) or f(int), f(x), f() */
    | direct_declarator '(' parameter_type_list ')'         { $$ = new FunctionDeclarator($1, $3); }
    | direct_declarator '(' identifier_list ')'             /* for old K&R functions */
    | direct_declarator '(' ')'                             { $$ = new FunctionDeclarator($1, NULL); }
    ;

pointer
    : '*'
    | '*' type_qualifier_list
    | '*' pointer
    | '*' type_qualifier_list pointer
    ;

type_qualifier_list
    : type_qualifier
    | type_qualifier_list type_qualifier
    ;


parameter_type_list
    : parameter_list                                        { $$ = $1; }
    | parameter_list ',' ELLIPSIS
    ;

parameter_list
    : parameter_declaration                                 { $$ = $1; }
    | parameter_list ',' parameter_declaration              { $$ = new FunctionParameterList($1, $3); }
    ;

parameter_declaration
    : declaration_specifiers declarator                     { $$ = new FunctionParameter($1, $2); }
    | declaration_specifiers abstract_declarator
    | declaration_specifiers                                { $$ = $1; }
    ;

identifier_list
    : IDENTIFIER                                            { $$ = new Identifier(*$1); }
    | identifier_list ',' IDENTIFIER
        { $$ = new BinaryNode($1, new Identifier(*$3)); }
    ;

type_name
    : specifier_qualifier_list
    | specifier_qualifier_list abstract_declarator
    ;

abstract_declarator
    : pointer
    | direct_abstract_declarator
    | pointer direct_abstract_declarator
    ;

direct_abstract_declarator
    : '(' abstract_declarator ')'
    | '[' ']'
    | '[' constant_expression ']'
    | direct_abstract_declarator '[' ']'
    | direct_abstract_declarator '[' constant_expression ']'
    | '(' ')'
    | '(' parameter_type_list ')'
    | direct_abstract_declarator '(' ')'
    | direct_abstract_declarator '(' parameter_type_list ')'
    ;

initializer
    : assignment_expression                                 { $$ = $1; }
    | '{' initializer_list '}'
    | '{' initializer_list ',' '}'
    ;

initializer_list
    : initializer
    | initializer_list ',' initializer
    ;

statement
    : labeled_statement                                     { $$ = $1; }
    | compound_statement                                    { $$ = $1; }
    | expression_statement                                  { $$ = $1; }
    | selection_statement                                   { $$ = $1; }
    | iteration_statement                                   { $$ = $1; }
    | jump_statement                                        { $$ = $1; }
    ;

labeled_statement
    : IDENTIFIER ':' statement
    | CASE constant_expression ':' statement
    | DEFAULT ':' statement
    ;

compound_statement
    : '{' '}'
    | '{' statement_list '}'                                { $$ = $2; }
    | '{' declaration_list '}'                              { $$ = $2; }
    | '{' declaration_list statement_list '}'               { $$ = new BinaryNode($2, $3); }
    ;

declaration_list
    : declaration                                           { $$ = $1; }
    | declaration_list declaration                          { $$ = new BinaryNode($1, $2); }
    ;

statement_list
    : statement                                             { $$ = $1; }
    | statement_list statement                              { $$ = new BinaryNode($1, $2); }
    ;

expression_statement
    : ';'
    | expression ';'                                        { $$ = $1; }
    ;

selection_statement
    : IF '(' expression ')' statement                       { $$ = new IfElse($3, $5); }
    | IF '(' expression ')' statement ELSE statement        { $$ = new IfElse($3, $5, $7); }
    | SWITCH '(' expression ')' statement
    ;

iteration_statement
    : WHILE '(' expression ')' statement
    | DO statement WHILE '(' expression ')' ';'
    | FOR '(' expression_statement expression_statement ')' statement
        { $$ = new For($3, $4, $6); }
    | FOR '(' expression_statement expression_statement expression ')' statement
        { $$ = new For($3, $4, $5, $7); }
    ;

jump_statement
    : GOTO IDENTIFIER ';'
    | CONTINUE ';'
    | BREAK ';'
    | RETURN ';'                                            { $$ = new Return(new Number(0)); }
    | RETURN expression ';'                                 { $$ = new Return($2); }
    ;

translation_unit
    : external_declaration                                  { $$ = $1; }
    | translation_unit external_declaration                 { $$ = new BinaryNode($1, $2); }
    ;

external_declaration
    : function_definition                                   { $$ = $1; }
    | declaration                                           { $$ = $1; }
    ;

function_definition
    : declaration_specifiers declarator declaration_list compound_statement
        /* This is the old K&R way function definition in C and can be ignored */
    | declaration_specifiers declarator compound_statement
        { $$ = new FunctionDefinition($1, $2, $3); }
    | declarator declaration_list compound_statement
    | declarator compound_statement
    ;

%%


const Node *g_root;


const Node *parseAST(std::string filename)
{
    yyin = fopen(filename.c_str(), "r");

    if (yyin == NULL)
    {
        std::cerr << "Couldn't open input file " << filename << std::endl;
        exit(1);
    }

    g_root = NULL;
    yyparse();

    return g_root;
}
