// References: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html

%code requires{
  #include "ast.hpp"

  #include <cassert>
  #include <string>

  extern FILE *yyin;
  extern const Node *g_root;

  // Declare functions provided by Flex,
  // so that Bison generated code can call them.
  void update_type_map(std::string id, Types type);
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
%type <string> IDENTIFIER STRING_LITERAL CHAR_LITERAL unary_operator

%type <node> primary_expression postfix_expression argument_expression_list
%type <node> unary_expression cast_expression
%type <node> multiplicative_expression additive_expression shift_expression
%type <node> relational_expression equality_expression and_expression
%type <node> exclusive_or_expression inclusive_or_expression
%type <node> logical_and_expression logical_or_expression
%type <node> conditional_expression assignment_expression assignment_operator
%type <node> expression constant_expression
%type <node> declaration declaration_specifiers
%type <node> init_declarator_list init_declarator
%type <node> storage_class_specifier type_specifier struct_declaration_list
%type <node> struct_declaration struct_or_union_specifier struct_or_union
%type <node> specifier_qualifier_list struct_declarator_list struct_declarator
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
    | STRING_LITERAL                                        { $$ = new String(*$1); }
    | CHAR_LITERAL                                          { $$ = new Char(*$1); }
    | '(' expression ')'                                    { $$ = $2; }
    ;

postfix_expression
    : primary_expression                                    { $$ = $1; }
    | postfix_expression '[' expression ']'                 { $$ = new ArrayAccess($1, $3); }
    | postfix_expression '(' ')'                            { $$ = new FunctionCall($1, NULL); }
    | postfix_expression '(' argument_expression_list ')'   { $$ = new FunctionCall($1, $3); }
    | postfix_expression '.' IDENTIFIER
        { $$ = new StructAccess($1, new Identifier(*$3)); }
    | postfix_expression PTR_OP IDENTIFIER
    | postfix_expression INC_OP                             { $$ = new PostIncrement($1); }
    | postfix_expression DEC_OP                             { $$ = new PostDecrement($1); }
    ;

argument_expression_list
    : assignment_expression
        { $$ = new FunctionArgumentList($1, NULL); }
    | argument_expression_list ',' assignment_expression
        { $$ = new FunctionArgumentList($1, $3); }
    ;

unary_expression
    : postfix_expression                    { $$ = $1; }
    | INC_OP unary_expression               { $$ = new PreIncrement($2); }
    | DEC_OP unary_expression
    | unary_operator cast_expression        { $$ = new UnaryExpression(*$1, $2); }
    | SIZEOF unary_expression               { $$ = new SizeOf($2); }
    | SIZEOF '(' type_name ')'              { $$ = new SizeOf($3); }
    ;

unary_operator
    : '&'                                   { $$ = new std::string("&"); }
    | '*'                                   { $$ = new std::string("*"); }
    | '+'                                   { $$ = new std::string("+"); }
    | '-'                                   { $$ = new std::string("-"); }
    | '~'                                   { $$ = new std::string("~"); }
    | '!'                                   { $$ = new std::string("!"); }
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
        { $$ = new LeftShift($1, $3); }
    | shift_expression RIGHT_OP additive_expression
        { $$ = new RightShift($1, $3); }
    ;

relational_expression
    : shift_expression                                      { $$ = $1; }
    | relational_expression '<' shift_expression
        { $$ = new LessThan($1, $3);}
    | relational_expression '>' shift_expression
        { $$ = new GreaterThan($1, $3);}
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
    : conditional_expression                                { $$ = $1; }
    ;

declaration
    : declaration_specifiers ';'                            { $$ = $1; }
    | type_define
        { $$ = new EmptyNode(); }
    | declaration_specifiers init_declarator_list ';'
        { $$ = new Declaration($1, $2); }
    ;

/*
    Parser "hack" in BIG quotation marks. By putting the typedefs before
    the Declaration, it will not create a declaration if it has detected
    it is a typedef.
*/
type_define
    : TYPEDEF type_specifier IDENTIFIER ';'
        { update_type_map(*$3, ((TypePtr)$2)->get_type()); }
    | TYPEDEF type_specifier '*' IDENTIFIER ';'
        { update_type_map(*$4, ((TypePtr)$2)->get_type()); }
    ;

declaration_specifiers
    : storage_class_specifier                               { $$ = $1; }
    | storage_class_specifier declaration_specifiers
    | type_specifier                                        { $$ = $1; }
    | type_specifier declaration_specifiers
    /* ignore qualifiers */
    | type_qualifier
    | type_qualifier declaration_specifiers
    ;

init_declarator_list
    : init_declarator                                       { $$ = $1; }
    | init_declarator_list ',' init_declarator
    ;

init_declarator
    : declarator                                            { $$ = $1; }
    | declarator '=' initializer
        { $$ = new Assign($1, new AssignOp("="), $3); }
    ;

storage_class_specifier
    : TYPEDEF                       /* Only this will be considered */
    | EXTERN
    | STATIC
    | AUTO
    | REGISTER
    ;

type_specifier
    : VOID                          { $$ = new BasicType(Types::VOID); }
    | CHAR                          { $$ = new BasicType(Types::CHAR); }
    | SHORT                         { $$ = new BasicType(Types::SHORT); }
    | INT                           { $$ = new BasicType(Types::INT); }
    | LONG                          { $$ = new BasicType(Types::LONG); }
    | FLOAT                         { $$ = new BasicType(Types::FLOAT); }
    | DOUBLE                        { $$ = new BasicType(Types::DOUBLE); }
    /* NOTE: This is a temporary solution- ignore compound types. */
    | SIGNED                        { $$ = new BasicType(Types::INT); }
    | UNSIGNED                      { $$ = new BasicType(Types::UNSIGNED_INT); }
    | struct_or_union_specifier     { $$ = $1; }
    | enum_specifier                { $$ = $1; }
    /* typedefs */
    | TYPE_NAME
    ;

struct_or_union_specifier
    : struct_or_union IDENTIFIER '{' struct_declaration_list '}'
        { $$ = new StructDefinition(*$2, $4); }
    | struct_or_union '{' struct_declaration_list '}'
    | struct_or_union IDENTIFIER
        { $$ = new StructInstance(*$2); }
    ;

struct_or_union
    : STRUCT
    | UNION                         /* ignore unions */
    ;

struct_declaration_list
    : struct_declaration
        { $$ = $1; }
    | struct_declaration_list struct_declaration
        /* No need for fancy stuff here */
        { $$ = new BinaryNode($1, $2); }
    ;

struct_declaration
    : specifier_qualifier_list struct_declarator_list ';'
        { $$ = new StructDeclaration($1, $2); }
    ;

specifier_qualifier_list
    : type_specifier specifier_qualifier_list
    | type_specifier
        { $$ = $1; }
    | type_qualifier specifier_qualifier_list
    | type_qualifier
    ;

struct_declarator_list
    : struct_declarator
        { $$ = $1; }
    | struct_declarator_list ',' struct_declarator
        { $$ = new StructDeclaratorList($1, $3); }
    ;

struct_declarator
    : declarator
        { $$ = $1; }
    | ':' constant_expression
    | declarator ':' constant_expression
    ;

enum_specifier
    : ENUM '{' enumerator_list '}'
        { $$ = new EnumDefinition($3); }
    | ENUM IDENTIFIER '{' enumerator_list '}'
        { $$ = new EnumDefinition(*$2, $4); }
    | ENUM IDENTIFIER
        // Does not exist yet
        // { $$ = new EnumInstance(*$2); }
    ;

enumerator_list
    : enumerator    { $$ = $1; }
    | enumerator_list ',' enumerator
        { $$ = new EnumList($1, $3); }
    ;

enumerator
    : IDENTIFIER
        { $$ = new EnumValue(*$1); }
    | IDENTIFIER '=' constant_expression
        { $$ = new EnumValue(*$1, $3); }
    ;

// Not considered in this implementation.
type_qualifier
    : CONST
    | VOLATILE
    ;

declarator
    : pointer direct_declarator                             { $$ = new PointerDeclarator($1, $2);}
    | direct_declarator                                     { $$ = $1; }
    ;

direct_declarator
    : IDENTIFIER
        { $$ = new VariableDeclarator(new Identifier(*$1)); }
    /* ^ Variable declarations */
    | '(' declarator ')'                                    { $$ = $2; }
    /* Array declarations with size or without size: arr[5] or arr[] */
    | direct_declarator '[' constant_expression ']'
        { $$ = new ArrayDeclarator($1, $3);}
    | direct_declarator '[' ']'
        { $$ = new ArrayDeclarator($1, NULL); }
    /* Function declarators like so: f(int x) or f(int), f(x), f() */
    | direct_declarator '(' parameter_type_list ')'         { $$ = new FunctionDeclarator($1, $3); }
    | direct_declarator '(' identifier_list ')'             /* for old K&R functions */
    | direct_declarator '(' ')'                             { $$ = new FunctionDeclarator($1, NULL); }
    ;

pointer
    : '*'                                                   { $$ = new Pointer(NULL); }
    | '*' type_qualifier_list
    | '*' pointer                                           { $$ = new Pointer($2); }
    | '*' type_qualifier_list pointer
    ;

// Not considered in this implementation
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
    : specifier_qualifier_list                              { $$ = $1; }
    | specifier_qualifier_list abstract_declarator
    ;

abstract_declarator
    : pointer                                               // { $$ = $1; }
    | direct_abstract_declarator                            // { $$ = $1; }
    | pointer direct_abstract_declarator                    // { $$ = new PointerDeclarator($1, NULL, $2); }
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
    : labeled_statement                         { $$ = $1; }
    | compound_statement                        { $$ = $1; }
    | expression_statement                      { $$ = $1; }
    | selection_statement                       { $$ = $1; }
    | iteration_statement                       { $$ = $1; }
    | jump_statement                            { $$ = $1; }
    ;

labeled_statement
    : IDENTIFIER ':' statement                  /* for goto */
    | CASE constant_expression ':' statement    { $$ = new Case($2, $4); }
    | DEFAULT ':' statement                     { $$ = new DefaultCase($3); }
    ;

// fuck
compound_statement
    : '{' '}'                                               { $$ = NULL; }
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
    | SWITCH '(' expression ')' statement                   { $$ = new Switch($3, $5); }
    ;

iteration_statement
    : WHILE '(' expression ')' statement
        { $$ = new While($3, $5);}
    | DO statement WHILE '(' expression ')' ';'
    | FOR '(' expression_statement expression_statement ')' statement
        { $$ = new For($3, $4, $6); }
    | FOR '(' expression_statement expression_statement expression ')' statement
        { $$ = new For($3, $4, $5, $7); }
    ;

jump_statement
    : GOTO IDENTIFIER ';'
    | CONTINUE ';'
    | BREAK ';'                             { $$ = new Break(); }
    | RETURN ';'                            { $$ = new Return(new Number(0)); }
    | RETURN expression ';'                 { $$ = new Return($2); }
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
