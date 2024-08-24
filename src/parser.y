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
  int yylex_destroy(void);
}

// All the possible types of tokens
%union{
    Node                *node;
    NodeList            *nodes;
    Type                *type;
    UnaryOpType         unary_op;
    AssignmentType      assignment_op;
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

%type <node> primary_expression postfix_expression unary_expression
%type <node> assignment_expression
%type <node> cast_expression
%type <node> multiplicative_expression additive_expression shift_expression
%type <node> relational_expression equality_expression and_expression
%type <node> exclusive_or_expression inclusive_or_expression
%type <node> logical_and_expression logical_or_expression
%type <node> conditional_expression
%type <node> expression constant_expression

%type <node> declaration
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
%type <nodes> init_declarator_list
%type <node> translation_unit initializer_list initializer

// Other types of nodes
%type <type> type_specifier declaration_specifiers specifier_qualifier_list
%type <node> struct_or_union_specifier
%type <assignment_op> assignment_operator
%type <node> declarator direct_declarator init_declarator
%type <node> compound_statement
%type <unary_op> unary_operator

%start root
%%

root
    : translation_unit
        { g_root = $1; }

primary_expression
	: IDENTIFIER
        { $$ = new Identifier(*$1); }
	| CONSTANT
        { $$ = new Constant(*$1); }
	| STRING_LITERAL
	| '(' expression ')'
	;

postfix_expression
	: primary_expression
        { $$ = $1; }
	| postfix_expression '[' expression ']'
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	| postfix_expression '.' IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP
        { $$ = new UnaryOp(UnaryOpType::POST_INC, $1); }
	| postfix_expression DEC_OP
        { $$ = new UnaryOp(UnaryOpType::POST_DEC, $1); }
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression
        { $$ = $1; }
	| INC_OP unary_expression
        { $$ = new UnaryOp(UnaryOpType::PRE_INC, $2); }
	| DEC_OP unary_expression
        { $$ = new UnaryOp(UnaryOpType::PRE_DEC, $2); }
	| unary_operator cast_expression
        { $$ = new UnaryOp($1, $2); }
	| SIZEOF unary_expression
	| SIZEOF '(' type_name ')'
	;

unary_operator
	: '&'
        { $$ = UnaryOpType::ADDRESS_OF; }
	| '*'
        { $$ = UnaryOpType::DEREFERENCE; }
	| '+'
        { $$ = UnaryOpType::PLUS; }
	| '-'
        { $$ = UnaryOpType::MINUS; }
	| '~'
        { $$ = UnaryOpType::BITWISE_NOT; }
	| '!'
        { $$ = UnaryOpType::LOGICAL_NOT; }
	;

cast_expression
	: unary_expression
        { $$ = $1; }
	| '(' type_name ')' cast_expression
	;

multiplicative_expression
	: cast_expression
        { $$ = $1; }
	| multiplicative_expression '*' cast_expression
        { $$ = new BinaryOp($1, $3, BinaryOpType::MUL); }
	| multiplicative_expression '/' cast_expression
        { $$ = new BinaryOp($1, $3, BinaryOpType::DIV); }
	| multiplicative_expression '%' cast_expression
        { $$ = new BinaryOp($1, $3, BinaryOpType::MOD); }
	;

additive_expression
	: multiplicative_expression
        { $$ = $1; }
	| additive_expression '+' multiplicative_expression
        { $$ = new BinaryOp($1, $3, BinaryOpType::ADD); }
	| additive_expression '-' multiplicative_expression
        { $$ = new BinaryOp($1, $3, BinaryOpType::SUB); }
	;

shift_expression
	: additive_expression
        { $$ = $1; }
	| shift_expression LEFT_OP additive_expression
        { $$ = new BinaryOp($1, $3, BinaryOpType::LSL); }
	| shift_expression RIGHT_OP additive_expression
        { $$ = new BinaryOp($1, $3, BinaryOpType::LSR); }
	;

relational_expression
	: shift_expression
        { $$ = $1; }
	| relational_expression '<' shift_expression
        { $$ = new BinaryOp($1, $3, BinaryOpType::LT); }
	| relational_expression '>' shift_expression
        { $$ = new BinaryOp($1, $3, BinaryOpType::GT); }
	| relational_expression LE_OP shift_expression
        { $$ = new BinaryOp($1, $3, BinaryOpType::LE); }
	| relational_expression GE_OP shift_expression
        { $$ = new BinaryOp($1, $3, BinaryOpType::GE); }
	;

equality_expression
	: relational_expression
        { $$ = $1; }
	| equality_expression EQ_OP relational_expression
        { $$ = new BinaryOp($1, $3, BinaryOpType::EQ); }
	| equality_expression NE_OP relational_expression
        { $$ = new BinaryOp($1, $3, BinaryOpType::NE); }
	;

and_expression
	: equality_expression
        { $$ = $1; }
	| and_expression '&' equality_expression
        { $$ = new BinaryOp($1, $3, BinaryOpType::BITWISE_AND); }
	;

exclusive_or_expression
	: and_expression
        { $$ = $1; }
	| exclusive_or_expression '^' and_expression
        { $$ = new BinaryOp($1, $3, BinaryOpType::BITWISE_XOR); }
	;

inclusive_or_expression
	: exclusive_or_expression
        { $$ = $1; }
	| inclusive_or_expression '|' exclusive_or_expression
        { $$ = new BinaryOp($1, $3, BinaryOpType::BITWISE_OR); }
	;

logical_and_expression
	: inclusive_or_expression
        { $$ = $1; }
	| logical_and_expression AND_OP inclusive_or_expression
        { $$ = new BinaryOp($1, $3, BinaryOpType::LOGICAL_AND); }
	;

logical_or_expression
	: logical_and_expression
        { $$ = $1; }
	| logical_or_expression OR_OP logical_and_expression
        { $$ = new BinaryOp($1, $3, BinaryOpType::LOGICAL_OR); }
	;

conditional_expression
	: logical_or_expression
        { $$ = $1; }
	| logical_or_expression '?' expression ':' conditional_expression
	;

assignment_expression
	: conditional_expression
        { $$ = $1; }
	| unary_expression assignment_operator assignment_expression
        { $$ = new Assignment($1, $3, $2); }
	;

assignment_operator
	: '='
        { $$ = AssignmentType::ASSIGN; }
	| MUL_ASSIGN
        { $$ = AssignmentType::MUL_ASSIGN; }
	| DIV_ASSIGN
        { $$ = AssignmentType::DIV_ASSIGN; }
	| MOD_ASSIGN
        { $$ = AssignmentType::MOD_ASSIGN; }
	| ADD_ASSIGN
        { $$ = AssignmentType::ADD_ASSIGN; }
	| SUB_ASSIGN
        { $$ = AssignmentType::SUB_ASSIGN; }
	| LEFT_ASSIGN
        { $$ = AssignmentType::LEFT_ASSIGN; }
	| RIGHT_ASSIGN
        { $$ = AssignmentType::RIGHT_ASSIGN; }
	| AND_ASSIGN
        { $$ = AssignmentType::AND_ASSIGN; }
	| XOR_ASSIGN
        { $$ = AssignmentType::XOR_ASSIGN; }
	| OR_ASSIGN
        { $$ = AssignmentType::OR_ASSIGN; }
	;

expression
	: assignment_expression
        { $$ = $1; }
	| expression ',' assignment_expression
	;

constant_expression
	: conditional_expression
        { $$ = $1; }
	;

declaration
	: declaration_specifiers ';'
        // Careful here: we need to render the ';'
        { $$ = new Declaration($1); }
	| declaration_specifiers init_declarator_list ';'
        { $$ = new Declaration($1, $2); }
	;

declaration_specifiers
	: storage_class_specifier
	| storage_class_specifier declaration_specifiers
	| type_specifier
        { $$ = $1; }
	| type_specifier declaration_specifiers
	| type_qualifier
	| type_qualifier declaration_specifiers
	;

init_declarator_list
	: init_declarator
        { $$ = new InitDeclaratorList($1); }
	| init_declarator_list ',' init_declarator
        { $1->push_back($3); $$ = $1; }
    ;

init_declarator
	: declarator
        { $$ = $1; }
	| declarator '=' initializer
        { $$ = new InitDeclarator($1, $3); }
    ;

storage_class_specifier
	: TYPEDEF
	| EXTERN
	| STATIC
	| AUTO
	| REGISTER
	;

type_specifier
	: VOID
        {$$ = new BasicType(Types::VOID); }
	| CHAR
        {$$ = new BasicType(Types::CHAR); }
	| SHORT
        {$$ = new BasicType(Types::SHORT); }
	| INT
        {$$ = new BasicType(Types::INT); }
	| LONG
        {$$ = new BasicType(Types::LONG); }
	| FLOAT
        {$$ = new BasicType(Types::FLOAT); }
	| DOUBLE
        {$$ = new BasicType(Types::DOUBLE); }
	| SIGNED
        {$$ = new BasicType(Types::INT); }
	| UNSIGNED
        {$$ = new BasicType(Types::UNSIGNED_INT); }
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
	| direct_declarator
        { $$ = $1; }
	;

direct_declarator
	: IDENTIFIER
        { $$ = new Identifier(*$1); }
	| '(' declarator ')'
        { $$ = $2; }
	| direct_declarator '[' constant_expression ']'
	| direct_declarator '[' ']'
	| direct_declarator '(' parameter_type_list ')'
        { $$ = new FunctionDeclarator($1, $3); }
	| direct_declarator '(' identifier_list ')'
	| direct_declarator '(' ')'
        { $$ = new FunctionDeclarator($1); }
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
	: parameter_list
        { $$ = $1; }
	| parameter_list ',' ELLIPSIS
        // Need <stdarg.h> for this
    ;

parameter_list
	: parameter_declaration
        { $$ = new FunctionParamList($1); }
	| parameter_list ',' parameter_declaration
        { $1->push_back($3); $$ = $1; }
    ;

parameter_declaration
	: declaration_specifiers declarator
        { $$ = new FunctionParam($1, $2); }
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
        { $$ = new FunctionParam($1); }
	;

identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER
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
	: assignment_expression
	| '{' initializer_list '}'
	| '{' initializer_list ',' '}'
	;

initializer_list
	: initializer
	| initializer_list ',' initializer
	;

statement
	: labeled_statement
        { $$ = new Statement($1); }
	| compound_statement
        { $$ = new Statement($1); }
    | expression_statement
        { $$ = new Statement($1); }
    | selection_statement
        { $$ = new Statement($1); }
    | iteration_statement
        { $$ = new Statement($1); }
    | jump_statement
        { $$ = new Statement($1); }
	;

labeled_statement
	: IDENTIFIER ':' statement
	| CASE constant_expression ':' statement
        { $$ = new Case($2, $4); }
	| DEFAULT ':' statement
        { $$ = new Default($3); }
	;

compound_statement
	: '{' '}'
        { $$ = new Scope(); }
	| '{' statement_list '}'
        { $$ = new Scope(NULL, $2); }
	| '{' declaration_list '}'
        { $$ = new Scope($2, NULL); }
	| '{' declaration_list statement_list '}'
        // This is a C90 quirk - declarations have to appear before.
        { $$ = new Scope($2, $3); }
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
	: ';'
        { $$ = new ExpressionStatement(); }
	| expression ';'
        { $$ = new ExpressionStatement($1); }
	;

selection_statement
	: IF '(' expression ')' statement
        { $$ = new If($3, $5); }
	| IF '(' expression ')' statement ELSE statement
        { $$ = new If($3, $5, $7); }
    | SWITCH '(' expression ')' statement
        { $$ = new Switch($3, $5); }
    ;

iteration_statement
	: WHILE '(' expression ')' statement
        { $$ = new While($3, $5); }
    | DO statement WHILE '(' expression ')' ';'
        { $$ = new DoWhile($2, $5); }
	| FOR '(' expression_statement expression_statement ')' statement
        { $$ = new For($3, $4, $6); }
    | FOR '(' expression_statement expression_statement expression ')' statement
        { $$ = new For($3, $4, $5, $7); }
    ;

jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';'
	| BREAK ';'
        { $$ = new Break(); }
	| RETURN ';'
        { $$ = new Return(); }
	| RETURN expression ';'
        { $$ = new Return($2); }
	;

translation_unit
	: external_declaration
        { $$ = $1; }
	| translation_unit external_declaration
	;

external_declaration
	: function_definition
        { $$ = $1; }
	| declaration
        { $$ = $1; }
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement
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
    fclose(yyin);
    yylex_destroy();

    return g_root;
}
