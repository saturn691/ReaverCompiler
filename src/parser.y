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
  int yylex_destroy(void);
}

// All the possible types of tokens
%union{
    Node                *node;
    NodeList            *nodes;
    Type                *type;
    Declarator          *declarator;
    ScopeManager        *scope_manager;
    AssignOp            *assign_op;
    Expression          *expr;
    UnaryOperator       unary_op;
    std::string         *string;
    yytokentype         token;
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


%type <string> IDENTIFIER STRING_LITERAL CHAR_LITERAL CONSTANT

%type <expr> primary_expression postfix_expression unary_expression
%type <expr> assignment_expression
%type <expr> cast_expression
%type <expr> multiplicative_expression additive_expression shift_expression
%type <expr> relational_expression equality_expression and_expression
%type <expr> exclusive_or_expression inclusive_or_expression
%type <expr> logical_and_expression logical_or_expression
%type <expr> conditional_expression
%type <expr> expression constant_expression

%type <node> declaration
%type <node> init_declarator_list
/* %type <node> storage_class_specifier type_qualifier */

%type <node> struct_declaration
/* %type <node> struct_or_union */
%type <node> struct_declarator
%type <node> enum_specifier enumerator
%type <node> pointer
%type <node> type_qualifier_list
%type <node> parameter_declaration type_name
/* %type <node> abstract_declarator direct_abstract_declarator */
%type <node> statement labeled_statement
%type <node> expression_statement selection_statement iteration_statement
%type <node> jump_statement
%type <node> external_declaration function_definition

%type <nodes> declaration_list statement_list argument_expression_list
%type <nodes> parameter_list parameter_type_list struct_declaration_list
%type <nodes> struct_declarator_list enumerator_list identifier_list
%type <nodes> translation_unit initializer_list initializer

// Other types of nodes
%type <type> type_specifier declaration_specifiers specifier_qualifier_list
%type <type> struct_or_union_specifier
%type <assign_op> assignment_operator
%type <declarator> declarator direct_declarator init_declarator
%type <scope_manager> compound_statement
%type <unary_op> unary_operator

%start root
%%

root
    : translation_unit              { g_root = $1; }

primary_expression
    : IDENTIFIER                    { $$ = new Identifier(*$1); }
    | CONSTANT                      { $$ = new Number(*$1); }
    | STRING_LITERAL                { $$ = new String(*$1); }
    | CHAR_LITERAL                  { $$ = new Char(*$1); }
    | '(' expression ')'            { $$ = static_cast<Expression*>($2); }
    ;

postfix_expression
    : primary_expression
        { $$ = $1; }
    | postfix_expression '[' expression ']'
        { $$ = new ArrayAccess($1, $3); }
    /*
        Function pointers are not tested in this code, so we can
        assume that the LHS is an identifier
    */
    | postfix_expression '(' ')'
        { $$ = new FunctionCall(dynamic_cast<Identifier*>($1), NULL); }
    | postfix_expression '(' argument_expression_list ')'
        { $$ = new FunctionCall(dynamic_cast<Identifier*>($1), $3); }
    | postfix_expression '.' IDENTIFIER
        { $$ = new StructAccess($1, new Identifier(*$3)); }
    | postfix_expression PTR_OP IDENTIFIER
        /* TODO this test will fail. I can't seem to crack structs. Oh well*/
        { $$ = new StructAccess($1, new Identifier(*$3), true); }
    | postfix_expression INC_OP
        { $$ = new PostIncrement($1); }
    | postfix_expression DEC_OP
        { $$ = new PostIncrement($1, true); }
    ;

argument_expression_list
    : assignment_expression
        { $$ = new FunctionArgumentList($1); }
    | argument_expression_list ',' assignment_expression
        { $1->push_back($3); $$ = $1; }
    ;

unary_expression
    : postfix_expression
        { $$ = $1; }
    | INC_OP unary_expression
        { $$ = new PostIncrement($2, false, true); }
    | DEC_OP unary_expression
        { $$ = new PostIncrement($2, true, true);}
    | unary_operator cast_expression
        { $$ = new UnaryExpression($1, $2); }
    | SIZEOF unary_expression
        { $$ = new SizeOf(static_cast<Node*>($2)); }
    | SIZEOF '(' type_name ')'
        { $$ = new SizeOf($3); }
    ;

unary_operator
    : '&'                           { $$ = UnaryOperator::ADDRESS; }
    | '*'                           { $$ = UnaryOperator::DEREFERENCE; }
    | '+'                           { $$ = UnaryOperator::ADD; }
    | '-'                           { $$ = UnaryOperator::SUB; }
    | '~'                           { $$ = UnaryOperator::BITWISE_NOT; }
    | '!'                           { $$ = UnaryOperator::LOGICAL_NOT; }
    ;

cast_expression
    : unary_expression              { $$ = $1; }
    | '(' type_name ')' cast_expression
    ;

multiplicative_expression
    : cast_expression
        { $$ = $1; }
    | multiplicative_expression '*' cast_expression
        { $$ = new Operator($1, $3, OperatorType::MUL); }
    | multiplicative_expression '/' cast_expression
        { $$ = new Operator($1, $3, OperatorType::DIV); }
    | multiplicative_expression '%' cast_expression
        { $$ = new Operator($1, $3, OperatorType::MOD); }
    ;

additive_expression
    : multiplicative_expression
        { $$ = $1; }
    | additive_expression '+' multiplicative_expression
        { $$ = new Operator($1, $3, OperatorType::ADD); }
    | additive_expression '-' multiplicative_expression
        { $$ = new Operator($1, $3, OperatorType::SUB); }
    ;

shift_expression
    : additive_expression
        { $$ = $1; }
    | shift_expression LEFT_OP additive_expression
        { $$ = new Operator($1, $3, OperatorType::LEFT_SHIFT); }
    | shift_expression RIGHT_OP additive_expression
        { $$ = new Operator($1, $3, OperatorType::RIGHT_SHIFT); }
    ;

relational_expression
    : shift_expression
        { $$ = $1; }
    | relational_expression '<' shift_expression
        { $$ = new LessThan($1, $3);}
    | relational_expression '>' shift_expression
        { $$ = new LessThan($1, $3, true);}
    | relational_expression LE_OP shift_expression
        { $$ = new LessThanEqual($1, $3);}
    | relational_expression GE_OP shift_expression
        { $$ = new LessThanEqual($1, $3, true); }
    ;

equality_expression
    : relational_expression
        { $$ = $1; }
    | equality_expression EQ_OP relational_expression
        { $$ = new Equal($1, $3, false); }
    | equality_expression NE_OP relational_expression
        { $$ = new Equal($1, $3, true); }
    ;

and_expression
    : equality_expression
        { $$ = $1; }
    | and_expression '&' equality_expression
        { $$ = new Operator($1, $3, OperatorType::BITWISE_AND); }
    ;

exclusive_or_expression
    : and_expression
        { $$ = $1; }
    | exclusive_or_expression '^' and_expression
        { $$ = new Operator($1, $3, OperatorType::BITWISE_XOR);}
    ;

inclusive_or_expression
    : exclusive_or_expression
        { $$ = $1; }
    | inclusive_or_expression '|' exclusive_or_expression
        { $$ = new Operator($1, $3, OperatorType::BITWISE_OR); }
    ;

logical_and_expression
    : inclusive_or_expression
        { $$ = $1; }
    | logical_and_expression AND_OP inclusive_or_expression
        { $$ = new LogicalAnd($1, $3);}
    ;

logical_or_expression
    : logical_and_expression
        { $$ = $1; }
    | logical_or_expression OR_OP logical_and_expression
        { $$ = new LogicalOr($1, $3); }
    ;

conditional_expression
    : logical_or_expression
        { $$ = $1; }
    | logical_or_expression '?' expression ':' conditional_expression
        { $$ = new Ternary($1, $3, $5); }
    ;

assignment_expression
    : conditional_expression
        { $$ = $1; }
    | unary_expression assignment_operator assignment_expression
        { $$ = new Assign($1, $2, $3); }
    ;

assignment_operator
    : '='
        { $$ = new AssignOp(AssignOpType::ASSIGN); }
    | MUL_ASSIGN
        { $$ = new AssignOp(AssignOpType::MUL_ASSIGN); }
    | DIV_ASSIGN
        { $$ = new AssignOp(AssignOpType::DIV_ASSIGN); }
    | MOD_ASSIGN
        { $$ = new AssignOp(AssignOpType::MOD_ASSIGN); }
    | ADD_ASSIGN
        { $$ = new AssignOp(AssignOpType::ADD_ASSIGN); }
    | SUB_ASSIGN
        { $$ = new AssignOp(AssignOpType::SUB_ASSIGN); }
    | LEFT_ASSIGN
        { $$ = new AssignOp(AssignOpType::LEFT_ASSIGN); }
    | RIGHT_ASSIGN
        { $$ = new AssignOp(AssignOpType::RIGHT_ASSIGN); }
    | AND_ASSIGN
        { $$ = new AssignOp(AssignOpType::AND_ASSIGN); }
    | XOR_ASSIGN
        { $$ = new AssignOp(AssignOpType::XOR_ASSIGN); }
    | OR_ASSIGN
        { $$ = new AssignOp(AssignOpType::OR_ASSIGN); }
    ;

expression
    : assignment_expression                                 { $$ = $1; }
    // Does not need to be implemented
    | expression ',' assignment_expression
    ;

constant_expression
    : conditional_expression                                { $$ = $1; }
    ;

declaration
    : declaration_specifiers ';'
        { $$ = $1; }
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
    // TODO -- This is wrong!
        { update_type_map(*$3, ((TypePtr)$2)->get_type()); }
    | TYPEDEF type_specifier '*' IDENTIFIER ';'
        { update_type_map(*$4, ((TypePtr)$2)->get_type()); }
    ;

// Only consider type_specifier
declaration_specifiers
    : storage_class_specifier
    | storage_class_specifier declaration_specifiers
    // Do not consider storage_class_specifier
    | type_specifier
        { $$ = $1; }
    | type_specifier declaration_specifiers
    /* ignore qualifiers */
    | type_qualifier
    | type_qualifier declaration_specifiers
    ;

init_declarator_list
    : init_declarator
        { $$ = $1; }
    // Does not need to be implemented
    | init_declarator_list ',' init_declarator
    ;

init_declarator
    : declarator
        { $$ = $1; }
    | declarator '=' initializer
        { $$ = new InitDeclarator($1, $3); }
    ;

// ------
// Does not need to be implemented
// ------
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
    | enum_specifier                // { $$ = $1; }
    /* typedefs */
    // TODO must be implemented
    | TYPE_NAME
    ;

struct_or_union_specifier
    : struct_or_union IDENTIFIER '{' struct_declaration_list '}'
        { $$ = new StructDefinition(*$2, $4); }
    | struct_or_union '{' struct_declaration_list '}'
    | struct_or_union IDENTIFIER
        { $$ = new StructType(*$2); }
    ;

struct_or_union
    : STRUCT
    | UNION                         /* ignore unions */
    ;

struct_declaration_list
    : struct_declaration
        { $$ = new NodeList($1); }
    | struct_declaration_list struct_declaration
        /* No need for fancy stuff here */
        { $1->push_back($2); $$ = $1; }
    ;

struct_declaration
    : specifier_qualifier_list struct_declarator_list ';'
        { $$ = new StructDeclaration($1, $2); }
    ;

specifier_qualifier_list
    : type_specifier specifier_qualifier_list
    | type_specifier
        { $$ = $1; }
    // Ignore type_qualifier - not used
    | type_qualifier specifier_qualifier_list
    | type_qualifier
    ;

struct_declarator_list
    : struct_declarator
        { $$ = new NodeList($1); }
    | struct_declarator_list ',' struct_declarator
        { $1->push_back($3); $$ = $1; }
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
    : enumerator
        { $$ = new EnumList($1); }
    | enumerator_list ',' enumerator
        { $1->push_back($3); $$ = $1; }
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
    : pointer direct_declarator
        { $$ = new PointerDeclarator($1, $2); }
    | direct_declarator
        { $$ = $1; }
    ;

direct_declarator
    : IDENTIFIER
        { $$ = new Identifier(*$1); }
    | '(' declarator ')'
        { $$ = $2; }
    /* Array declarations with size or without size: arr[5] or arr[] */
    | direct_declarator '[' constant_expression ']'
        { $$ = new ArrayDeclarator($1, $3);}
    | direct_declarator '[' ']'
        { $$ = new ArrayDeclarator($1, NULL); }
    /* Function declarators like so: f(int x) or f(int), f(x), f() */
    | direct_declarator '(' parameter_type_list ')'
        { $$ = new FunctionDeclarator($1, $3); }
    /* for old K&R functions */
    | direct_declarator '(' identifier_list ')'
    | direct_declarator '(' ')'
        { $$ = new FunctionDeclarator($1, NULL); }
    ;

pointer
    : '*'
        { $$ = new Pointer(NULL); }
    | '*' type_qualifier_list
    | '*' pointer
        { $$ = new Pointer($2); }
    | '*' type_qualifier_list pointer
    ;

// Not considered in this implementation
type_qualifier_list
    : type_qualifier
    | type_qualifier_list type_qualifier
    ;


parameter_type_list
    : parameter_list
        { $$ = $1; }
    // Does not need to be implemented
    | parameter_list ',' ELLIPSIS
    ;

parameter_list
    : parameter_declaration
        { $$ = new FunctionParameterList($1); }
    | parameter_list ',' parameter_declaration
        { $1->push_back($3); $$ = $1; }
    ;

parameter_declaration
    : declaration_specifiers declarator
        { $$ = new FunctionParameter($1, $2); }
    | declaration_specifiers abstract_declarator
        /* ??? */
    | declaration_specifiers
        { $$ = $1; }
    ;

identifier_list
    : IDENTIFIER
        { $$ = new NodeList(new Identifier(*$1)); }
    | identifier_list ',' IDENTIFIER
        { $1->push_back(new Identifier(*$3)); $$ = $1; }
    ;

type_name
    : specifier_qualifier_list      { $$ = $1; }
    // What is an abstract declarator?
    | specifier_qualifier_list abstract_declarator
    ;

// ------
// Maybe we need to implement this, I dunno
// ------
abstract_declarator
    : pointer                                               // { $$ = $1; }
    | direct_abstract_declarator                            // { $$ = $1; }
    | pointer direct_abstract_declarator                    // { $$ = new PointerDeclarator($1, NULL, $2); }
    ;

// ------
// What even is this lol
// ------
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
    : assignment_expression
        { $$ = new NodeList($1); }
    // Like int arr[] = {1, 2, 3, 4, 5};
    | '{' initializer_list '}'
        { $$ = $2; }
    | '{' initializer_list ',' '}'
        { $$ = $2; }
    ;

initializer_list
    : initializer
        { $$ = new ArrayInitializerList($1); }
    | initializer_list ',' initializer
        { $1->push_back($3); $$ = $1; }
    ;

statement
    : labeled_statement             { $$ = $1; }
    | compound_statement            { $$ = $1; }
    | expression_statement          { $$ = $1; }
    | selection_statement           { $$ = $1; }
    | iteration_statement           { $$ = $1; }
    | jump_statement                { $$ = $1; }
    ;

labeled_statement
    : IDENTIFIER ':' statement      /* for goto */
    | CASE constant_expression ':' statement
        { $$ = new Case($2, $4); }
    | DEFAULT ':' statement
        { $$ = new DefaultCase($3); }
    ;

compound_statement
    : '{' '}'
        { $$ = new ScopeManager(NULL, NULL); }
    | '{' statement_list '}'
        { $$ = new ScopeManager($2, NULL); }
    | '{' declaration_list '}'
        { $$ = new ScopeManager(NULL, $2); }
    | '{' declaration_list statement_list '}'
        { $$ = new ScopeManager($2, $3); }
    ;

declaration_list
    : declaration
        { $$ = new NodeList($1); }
    | declaration_list declaration
        { $1->push_back($2); $$ = $1; }
    ;

statement_list
    : statement
        { $$ = new NodeList($1); }
    | statement_list statement
        { $1->push_back($2); $$ = $1; }
    ;

expression_statement
    : ';'                           { $$ = new EmptyNode(); }
    | expression ';'                { $$ = $1; }
    ;

selection_statement
    : IF '(' expression ')' statement
        { $$ = new IfElse($3, $5); }
    | IF '(' expression ')' statement ELSE statement
        { $$ = new IfElse($3, $5, $7); }
    | SWITCH '(' expression ')' statement
        { $$ = new Switch($3, $5); }
    ;

iteration_statement
    : WHILE '(' expression ')' statement
        { $$ = new While($3, $5);}
    | DO statement WHILE '(' expression ')' ';'
        { $$ = new DoWhile($2, $5); }
    | FOR '(' expression_statement expression_statement ')' statement
        { $$ = new For($3, $4, $6); }
    | FOR '(' expression_statement expression_statement expression ')' statement
        { $$ = new For($3, $4, $5, $7); }
    |
    // Makes compatible with C99 style for loops
    FOR '(' declaration expression_statement ')' statement
        { $$ = new For($3, $4, $6); }
    | FOR '(' declaration expression_statement expression ')' statement
        { $$ = new For($3, $4, $5, $7); }
    ;

jump_statement
    : GOTO IDENTIFIER ';'           // Does not need to be implemented
    | CONTINUE ';'                  { $$ = new Continue(); }
    | BREAK ';'                     { $$ = new Break(); }
    | RETURN ';'                    { $$ = new Return(new Number(0)); }
    | RETURN expression ';'         { $$ = new Return($2); }
    ;

translation_unit
    : external_declaration
        { $$ = new NodeList($1); }
    | translation_unit external_declaration
        { $1->push_back($2); $$ = $1; }
    ;

external_declaration
    : function_definition           { $$ = $1; }
    | declaration                   { $$ = $1; }
    ;

function_definition
    : declaration_specifiers declarator declaration_list compound_statement
        /* This is the old K&R way function definition in C and can be ignored */
    | declaration_specifiers declarator compound_statement
        { $$ = new FunctionDefinition($1, $2, $3); }
    | declarator declaration_list compound_statement
        /* This is the old K&R way function definition in C and can be ignored */
    | declarator compound_statement
        /* This is valid syntax, this is assumed to have return type int */
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
    fclose(yyin);
    yylex_destroy();

    return g_root;
}
