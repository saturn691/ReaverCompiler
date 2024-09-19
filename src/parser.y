// References: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html

%code requires{
  #include "ast/ast.hpp"
  #include "ty/ty.hpp"

  #include <cassert>
  #include <variant>
  #include <string>

  using namespace ast;

  extern FILE *yyin;
  extern const TranslationUnit *g_root;

  // Declare functions provided by Flex,
  // so that Bison generated code can call them.
  int yylex(void);
  void yyerror(const char *);
  int yylex_destroy(void);
}

// All the possible types of tokens
%union{
    Node                    *node;
    NodeList<Node>          *nodes;

    Type                    *type;
    Declarator              *decl;
    InitDeclarator          *i_decl;
    Expression              *expr;
    FunctionDefinition      *func_def;
    FunctionDeclarator      *func_decl;
    Declaration             *declar;
    DeclarationNode         *decl_node;
    CompoundStatement       *scope;
    Statement               *stmt;

    // UnaryOpType             unary_op;
    AssignmentType          assignment_op;
    // StructOrUnionType       struct_or_union;

    // ArrayInitializerList    *array_initializer_list;
    DeclarationList         *declaration_list;
    // EnumList                *enum_list;
    // FunctionCallList        *function_call_list;
    FunctionParamList       *function_param_list;
    InitDeclaratorList      *init_decl_list;
    StatementList           *stmt_list;
    // StructDeclarationList   *struct_declaration_list;
    // StructItemList          *struct_item_list;
    TranslationUnit         *translation_unit;

    std::variant<DeclarationNode*, FunctionDefinition*> *ext_decl;
    std::string             *string;
    int                     integer;
    yytokentype             token;
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
%type <expr> initializer

/* %type <node> storage_class_specifier type_qualifier */

%type <node> struct_declaration
%type <ext_decl> external_declaration
%type <struct_or_union> struct_or_union
%type <node> struct_declarator enumerator
%type <type> enum_specifier
%type <integer> pointer
%type <node> type_qualifier_list
%type <node> parameter_declaration type_name
/* %type <node> abstract_declarator direct_abstract_declarator */

%type <stmt> statement labeled_statement
%type <stmt> expression_statement selection_statement iteration_statement
%type <stmt> jump_statement

%type <nodes> identifier_list

%type <declaration_list> declaration_list
%type <enum_list> enumerator_list
%type <init_decl_list> init_declarator_list
%type <function_param_list> parameter_list parameter_type_list
%type <array_initializer_list> initializer_list
%type <function_call_list> argument_expression_list
%type <stmt_list> statement_list
%type <struct_item_list> struct_declarator_list
%type <struct_declaration_list> struct_declaration_list
%type <translation_unit> translation_unit

// Other types of nodes
%type <type> type_specifier declaration_specifiers specifier_qualifier_list
%type <type> struct_or_union_specifier
%type <assignment_op> assignment_operator
%type <decl> declarator direct_declarator
%type <i_decl> init_declarator
%type <scope> compound_statement
%type <unary_op> unary_operator
%type <func_def> function_definition
%type <decl_node> declaration
%type <func_decl> function_declarator


%start root
%%

root
    : translation_unit
        { g_root = $1; }

primary_expression
	: IDENTIFIER
        { $$ = new Identifier(std::string(*$1)); }
	| CONSTANT
        { $$ = new Constant(*$1); }
	| STRING_LITERAL
        /* { $$ = new String(*$1); } */
	| CHAR_LITERAL
        /* { $$ = new Char(*$1); } */
    | '(' expression ')'
        /* { $$ = new Parenthesis($2); } */
	;

postfix_expression
	: primary_expression
        { $$ = $1; }
	| postfix_expression '[' expression ']'
        /* { $$ = new ArrayAccess($1, $3); } */
	| postfix_expression '(' ')'
        /* { $$ = new FunctionCall($1, new FunctionCallList()); } */
	| postfix_expression '(' argument_expression_list ')'
        /* { $$ = new FunctionCall($1, $3); } */
    | postfix_expression '.' IDENTIFIER
        /* { $$ = new StructAccess($1, StructAccessType::DOT, *$3); } */
    | postfix_expression PTR_OP IDENTIFIER
        /* { $$ = new StructAccess($1, StructAccessType::ARROW, *$3); } */
	| postfix_expression INC_OP
        /* { $$ = new UnaryOp(UnaryOpType::POST_INC, $1); } */
	| postfix_expression DEC_OP
        /* { $$ = new UnaryOp(UnaryOpType::POST_DEC, $1); } */
	;

argument_expression_list
	: assignment_expression
        /* { $$ = new FunctionCallList($1); } */
	| argument_expression_list ',' assignment_expression
        /* { $1->push_back($3); $$ = $1; } */
    ;

unary_expression
	: postfix_expression
        { $$ = $1; }
	| INC_OP unary_expression
        /* { $$ = new UnaryOp(UnaryOpType::PRE_INC, $2); } */
	| DEC_OP unary_expression
        /* { $$ = new UnaryOp(UnaryOpType::PRE_DEC, $2); } */
	| unary_operator cast_expression
        /* { $$ = new UnaryOp($1, $2); } */
	| SIZEOF unary_expression
        /* { $$ = new Sizeof($2); } */
    | SIZEOF '(' type_name ')'
        /* { $$ = new Sizeof($3); } */
	;

unary_operator
	: '&'
        /* { $$ = UnaryOpType::ADDRESS_OF; } */
	| '*'
        /* { $$ = UnaryOpType::DEREFERENCE; } */
	| '+'
        /* { $$ = UnaryOpType::PLUS; } */
	| '-'
        /* { $$ = UnaryOpType::MINUS; } */
	| '~'
        /* { $$ = UnaryOpType::BITWISE_NOT; } */
	| '!'
        /* { $$ = UnaryOpType::LOGICAL_NOT; } */
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
        { $$ = new Assignment($1, $2, $3); }
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
        { $$ = new DeclarationNode($1); }
	| declaration_specifiers init_declarator_list ';'
        { $$ = new DeclarationNode($1, $2); }
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
        { $$ = new InitDeclarator($1); }
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
        {$$ = new BasicType(ty::Types::VOID); }
	| CHAR
        {$$ = new BasicType(ty::Types::CHAR); }
	| SHORT
        {$$ = new BasicType(ty::Types::SHORT); }
	| INT
        {$$ = new BasicType(ty::Types::INT); }
	| LONG
        {$$ = new BasicType(ty::Types::LONG); }
	| FLOAT
        {$$ = new BasicType(ty::Types::FLOAT); }
	| DOUBLE
        {$$ = new BasicType(ty::Types::DOUBLE); }
	| SIGNED
        {$$ = new BasicType(ty::Types::INT); }
	| UNSIGNED
        {$$ = new BasicType(ty::Types::UNSIGNED_INT); }
	| struct_or_union_specifier
        { $$ = $1; }
	| enum_specifier
        { $$ = $1; }
	| TYPE_NAME
	;

struct_or_union_specifier
	: struct_or_union IDENTIFIER '{' struct_declaration_list '}'
        /* { $$ = new StructDeclaration($1, *$2, $4); } */
    | struct_or_union '{' struct_declaration_list '}'
        /* { $$ = new StructDeclaration($1, $3); } */
    | struct_or_union IDENTIFIER
        /* { $$ = new Struct($1, *$2); } */
    ;

struct_or_union
	: STRUCT
        /* { $$ = StructOrUnionType::STRUCT; } */
	| UNION
        /* { $$ = StructOrUnionType::UNION; } */
	;

struct_declaration_list
	: struct_declaration
        /* { $$ = new StructDeclarationList(dynamic_cast<const StructItem*>($1)); } */
	| struct_declaration_list struct_declaration
        /* { $1->push_back(dynamic_cast<const StructItem*>($2)); $$ = $1; } */
    ;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'
        /* { $$ = new StructItem($1, $2); } */
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
        /* { $$ = new StructItemList(dynamic_cast<const StructItemDeclarator*>($1)); } */
	| struct_declarator_list ',' struct_declarator
        /* { $1->push_back(dynamic_cast<const StructItemDeclarator*>($3)); $$ = $1; } */
    ;

struct_declarator
	: declarator
        /* { $$ = new StructItemDeclarator($1); } */
	| ':' constant_expression
        // Anonymous bitfield
	| declarator ':' constant_expression
        //  Named bitfield
    ;

enum_specifier
	: ENUM '{' enumerator_list '}'
        /* { $$ = new EnumDeclaration($3); } */
	| ENUM IDENTIFIER '{' enumerator_list '}'
        /* { $$ = new EnumDeclaration(*$2, $4); } */
    | ENUM IDENTIFIER
        /* { $$ = new Enum(*$2); } */
	;

enumerator_list
	: enumerator
        /* { $$ = new EnumList(dynamic_cast<const EnumItem*>($1)); } */
	| enumerator_list ',' enumerator
        /* { $1->push_back(dynamic_cast<const EnumItem*>($3)); $$ = $1; } */
	;

enumerator
	: IDENTIFIER
        /* { $$ = new EnumItem(*$1); } */
	| IDENTIFIER '=' constant_expression
        /* { $$ = new EnumItem(*$1, $3); } */
    ;

type_qualifier
	: CONST
	| VOLATILE
	;

/* Makes lowering easier */
function_declarator
    : direct_declarator '(' parameter_type_list ')'
        { $$ = new FunctionDeclarator($1, $3); }
    | direct_declarator '(' ')'
        { $$ = new FunctionDeclarator($1, nullptr); }
	| direct_declarator '(' identifier_list ')'
        // Old K&R style function declaration
    | pointer direct_declarator '(' parameter_type_list ')'
        /* { $$ = new FunctionDeclarator($2, $4, $1); } */
    | pointer direct_declarator '(' ')'
        /* { $$ = new FunctionDeclarator($2, nullptr, $1); } */
    ;

declarator
	: pointer direct_declarator
        /* { $$ = new PointerDeclarator($1, $2); } */
    | direct_declarator
        { $$ = $1; }
	;

direct_declarator
	: IDENTIFIER
        { $$ = new Identifier(std::string(*$1)); }
	| '(' declarator ')'
        { $$ = $2; }
	| direct_declarator '[' constant_expression ']'
        /* { $$ = new ArrayDeclarator($1, $3); } */
    | direct_declarator '[' ']'
        /* { $$ = new ArrayDeclarator($1); } */
    ;

// Ignore const/volatile for now
pointer
	: '*'
        { $$ = 1; }
	| '*' type_qualifier_list
        { $$ = 1; }
    | '*' pointer
        { $$ = $2 + 1; }
	| '*' type_qualifier_list pointer
        { $$ = $3 + 1; }
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
        { $$ = new FunctionParamList(dynamic_cast<const FunctionParam*>($1)); }
	| parameter_list ',' parameter_declaration
        { $1->push_back(dynamic_cast<const FunctionParam*>($3)); $$ = $1; }
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

// Doesn't change anything in lowering but keeps parser happy
initializer
	: assignment_expression
        { $$ = $1; }
	| '{' initializer_list '}'
        /* { $$ = new NodeList<Node>($2); } */
    | '{' initializer_list ',' '}'
        /* { $$ = new NodeList<Node>($2); } */
    ;

initializer_list
	: initializer
        /* { $$ = new ArrayInitializerList($1); } */
	| initializer_list ',' initializer
        /* { $1->push_back($3); $$ = $1; } */
	;

statement
	: labeled_statement
        { $$ = $1; }
	| compound_statement
        { $$ = $1; }
    | expression_statement
		{ $$ = $1; }
    | selection_statement
		{ $$ = $1; }
	| iteration_statement
		{ $$ = $1; }
	| jump_statement
		{ $$ = $1; }
	;

labeled_statement
	: IDENTIFIER ':' statement
	| CASE constant_expression ':' statement
        /* { $$ = new Case($2, $4); } */
	| DEFAULT ':' statement
        /* { $$ = new Default($3); } */
	;

compound_statement
	: '{' '}'
        { $$ = new CompoundStatement(); }
	| '{' statement_list '}'
        { $$ = new CompoundStatement(NULL, $2); }
	| '{' declaration_list '}'
        { $$ = new CompoundStatement($2, NULL); }
	| '{' declaration_list statement_list '}'
        // This is a C90 quirk - declarations have to appear before.
        { $$ = new CompoundStatement($2, $3); }
	;

declaration_list
	: declaration
        { $$ = new DeclarationList($1); }
	| declaration_list declaration
        { $1->push_back($2); $$ = $1; }
	;

statement_list
	: statement
        { $$ = new StatementList(dynamic_cast<const Statement*>($1)); }
	| statement_list statement
        { $1->push_back(dynamic_cast<const Statement*>($2)); $$ = $1; }
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
        /* { $$ = new Switch($3, $5); } */
    ;

iteration_statement
	: WHILE '(' expression ')' statement
        /* { $$ = new While($3, $5); } */
    | DO statement WHILE '(' expression ')' ';'
        /* { $$ = new DoWhile($2, $5); } */
	| FOR '(' expression_statement expression_statement ')' statement
        /* { $$ = new For($3, $4, $6); } */
    | FOR '(' expression_statement expression_statement expression ')' statement
        /* { $$ = new For($3, $4, $5, $7); } */
    ;

jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';'
	| BREAK ';'
        /* { $$ = new Break(); } */
	| RETURN ';'
        { $$ = new Return(); }
	| RETURN expression ';'
        { $$ = new Return($2); }
	;

translation_unit
	: external_declaration
        { $$ = new TranslationUnit($1); }
	| translation_unit external_declaration
        { $1->push_back($2); $$ = $1; }
    ;

external_declaration
	: function_definition
        { $$ = new std::variant<DeclarationNode*, FunctionDefinition*>($1); }
	| declaration
        { $$ = new std::variant<DeclarationNode*, FunctionDefinition*>($1); }
	;

function_definition
	: declaration_specifiers function_declarator declaration_list compound_statement
	| declaration_specifiers function_declarator compound_statement
        { $$ = new FunctionDefinition($1, $2, $3); }
	| declarator declaration_list compound_statement
	| declarator compound_statement
	;

%%

const TranslationUnit *g_root;

namespace ast
{
    const TranslationUnit *parseAST(std::string filename)
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
}
