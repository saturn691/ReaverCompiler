// Reference: https://www.quut.com/c/ANSI-C-grammar-y-1999.html

%code requires{
    #include "AST/AST.hpp"

    #include <string>

    using namespace AST;

    extern FILE *yyin;
    extern const TranslationUnit *g_root;

    // Declare functions provided by Flex,
    // so that Bison generated code can call them.
    int yylex(void);
    void yyerror(const char *);
    int yylex_destroy(void);
}

%union {
    TranslationUnit                     *tu;
    FnDef                  				*func_def;
    DeclNode                     		*decl_node;
	
	std::variant<DeclNode*, Stmt*>		*block_item_;
	BlockItemList					   	*block_item_list_;
	CompoundStmt					   	*compound_stmt;
	ParamDecl					   		*param_decl;
	ParamList					   		*param_list;
	
	Decl                         		*decl;
	Expr 					   			*expr;
	Stmt 					   			*stmt;
	TypeNode				  			*type;
    
	std::variant<DeclNode*, FnDef*>     *ext_decl;

    std::string                         *string;
    int                                 integer;
    yytokentype                         token;
}


%token IDENTIFIER CONSTANT STRING_LITERAL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN TYPE_NAME

%token TYPEDEF EXTERN STATIC AUTO REGISTER INLINE RESTRICT
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token BOOL COMPLEX IMAGINARY
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%type <string> IDENTIFIER CONSTANT STRING_LITERAL

%type <tu> translation_unit
%type <func_def> function_definition
%type <block_item_> block_item
%type <block_item_list_> block_item_list
%type <compound_stmt> compound_statement
%type <decl_node> declaration
%type <ext_decl> external_declaration
%type <decl> declarator direct_declarator
%type <param_decl> parameter_declaration
%type <param_list> parameter_list parameter_type_list
%type <type> type_specifier declaration_specifiers specifier_qualifier_list
%type <expr> primary_expression postfix_expression unary_expression
%type <expr> cast_expression multiplicative_expression additive_expression
%type <expr> shift_expression relational_expression equality_expression
%type <expr> and_expression exclusive_or_expression inclusive_or_expression
%type <expr> logical_and_expression logical_or_expression
%type <expr> conditional_expression assignment_expression expression
%type <stmt> statement labeled_statement jump_statement
%type <stmt> expression_statement selection_statement iteration_statement


%start root
%%

root
	: translation_unit
		{ g_root = $1; }
	;

primary_expression
	: IDENTIFIER
		{ $$ = new Identifier(std::string(*$1)); }
	| CONSTANT
		{ $$ = new Constant(std::string(*$1)); }
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
	| postfix_expression DEC_OP
	| '(' type_name ')' '{' initializer_list '}'
	| '(' type_name ')' '{' initializer_list ',' '}'
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression
		{ $$ = $1; }
	| INC_OP unary_expression
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
	: unary_expression
		{ $$ = $1; }
	| '(' type_name ')' cast_expression
	;

multiplicative_expression
	: cast_expression
	 	{ $$ = $1; }
	| multiplicative_expression '*' cast_expression
	| multiplicative_expression '/' cast_expression
	| multiplicative_expression '%' cast_expression
	;

additive_expression
	: multiplicative_expression
		{ $$ = $1; }
	| additive_expression '+' multiplicative_expression
		{ $$ = new BinaryOp($1, $3, BinaryOp::Op::ADD); }
	| additive_expression '-' multiplicative_expression
		{ $$ = new BinaryOp($1, $3, BinaryOp::Op::SUB); }
	;

shift_expression
	: additive_expression
		{ $$ = $1; }
	| shift_expression LEFT_OP additive_expression
	| shift_expression RIGHT_OP additive_expression
	;

relational_expression
	: shift_expression
		{ $$ = $1; }
	| relational_expression '<' shift_expression
	| relational_expression '>' shift_expression
	| relational_expression LE_OP shift_expression
	| relational_expression GE_OP shift_expression
	;

equality_expression
	: relational_expression
		{ $$ = $1; }
	| equality_expression EQ_OP relational_expression
	| equality_expression NE_OP relational_expression
	;

and_expression
	: equality_expression
		{ $$ = $1; }
	| and_expression '&' equality_expression
	;

exclusive_or_expression
	: and_expression
		{ $$ = $1; }
	| exclusive_or_expression '^' and_expression
	;

inclusive_or_expression
	: exclusive_or_expression
		{ $$ = $1; }
	| inclusive_or_expression '|' exclusive_or_expression
	;

logical_and_expression
	: inclusive_or_expression
		{ $$ = $1; }
	| logical_and_expression AND_OP inclusive_or_expression
	;

logical_or_expression
	: logical_and_expression
		{ $$ = $1; }
	| logical_or_expression OR_OP logical_and_expression
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
	;

assignment_operator
	: '='
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
	| AND_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
	;

expression
	: assignment_expression
		{ $$ = $1; }
	| expression ',' assignment_expression
	;

constant_expression
	: conditional_expression
	;

declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';'
	;

declaration_specifiers
	: storage_class_specifier
	| storage_class_specifier declaration_specifiers
	| type_specifier
		{ $$ = $1; }
	| type_specifier declaration_specifiers
	| type_qualifier
	| type_qualifier declaration_specifiers
	| function_specifier
	| function_specifier declaration_specifiers
	;

init_declarator_list
	: init_declarator
	| init_declarator_list ',' init_declarator
	;

init_declarator
	: declarator
	| declarator '=' initializer
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
		{ $$ = new BasicType(Types::VOID); }
	| CHAR
		{ $$ = new BasicType(Types::CHAR); }
	| SHORT
		{ $$ = new BasicType(Types::SHORT); }
	| INT
		{ $$ = new BasicType(Types::INT); }
	| LONG
		{ $$ = new BasicType(Types::LONG); }
	| FLOAT
		{ $$ = new BasicType(Types::FLOAT); }
	| DOUBLE
		{ $$ = new BasicType(Types::DOUBLE); }
	| SIGNED
		// Typically used as an extension, but temp solution for now
		{ $$ = new BasicType(Types::INT); }
	| UNSIGNED
		{ $$ = new BasicType(Types::UNSIGNED_INT); }
	| BOOL
		{ $$ = new BasicType(Types::BOOL); }
	| COMPLEX
		{ $$ = new BasicType(Types::COMPLEX); }
	| IMAGINARY
		{ $$ = new BasicType(Types::IMAGINARY); }
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
	| ENUM '{' enumerator_list ',' '}'
	| ENUM IDENTIFIER '{' enumerator_list ',' '}'
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
	| RESTRICT
	| VOLATILE
	;

function_specifier
	: INLINE
	;

declarator
	: pointer direct_declarator
	| direct_declarator
		{ $$ = $1; }
	;


direct_declarator
	: IDENTIFIER
		{ $$ = new Identifier(std::string(*$1)); }
	| '(' declarator ')'
	| direct_declarator '[' type_qualifier_list assignment_expression ']'
	| direct_declarator '[' type_qualifier_list ']'
	| direct_declarator '[' assignment_expression ']'
	| direct_declarator '[' STATIC type_qualifier_list assignment_expression ']'
	| direct_declarator '[' type_qualifier_list STATIC assignment_expression ']'
	| direct_declarator '[' type_qualifier_list '*' ']'
	| direct_declarator '[' '*' ']'
	| direct_declarator '[' ']'
	| direct_declarator '(' parameter_type_list ')'
		{ $$ = new FnDecl($1, $3); }
	| direct_declarator '(' identifier_list ')'
		/* K&R style */
	| direct_declarator '(' ')'
		{ $$ = new FnDecl($1); }
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
		/* requires stdarg.h */
	;

parameter_list
	: parameter_declaration
		{ $$ = new ParamList($1); }
	| parameter_list ',' parameter_declaration
		{ $1->pushBack($3); $$ = $1; }
	;

parameter_declaration
	: declaration_specifiers declarator
		{ $$ = new ParamDecl($1, $2); }
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
		{ $$ = new ParamDecl($1); }
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
	| '[' assignment_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' assignment_expression ']'
	| '[' '*' ']'
	| direct_abstract_declarator '[' '*' ']'
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
	| designation initializer
	| initializer_list ',' initializer
	| initializer_list ',' designation initializer
	;

designation
	: designator_list '='
	;

designator_list
	: designator
	| designator_list designator
	;

designator
	: '[' constant_expression ']'
	| '.' IDENTIFIER
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
	| DEFAULT ':' statement
	;

compound_statement
	: '{' '}'
		{ $$ = new CompoundStmt(); }
	| '{' block_item_list '}'
		{ $$ = new CompoundStmt($2); }
	;

block_item_list
	: block_item
		{ $$ = new BlockItemList($1); }
	| block_item_list block_item
	 	{ $1->pushBack($2); $$ = $1; }
	;

block_item
	: declaration
		{ $$ = new std::variant<DeclNode*, Stmt*>($1); }
	| statement
		{ $$ = new std::variant<DeclNode*, Stmt*>($1); }
	;

expression_statement
	: ';'
	| expression ';'
	;

selection_statement
	: IF '(' expression ')' statement
	| IF '(' expression ')' statement ELSE statement
	| SWITCH '(' expression ')' statement
	;

iteration_statement
	: WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement expression_statement ')' statement
	| FOR '(' expression_statement expression_statement expression ')' statement
	| FOR '(' declaration expression_statement ')' statement
	| FOR '(' declaration expression_statement expression ')' statement
	;

jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';'
	| BREAK ';'
	| RETURN ';'
		{ $$ = new Return(); }
	| RETURN expression ';'
		{ $$ = new Return($2); }
	;

translation_unit
	: external_declaration
        { $$ = new TranslationUnit($1); }
	| translation_unit external_declaration
        { $1->pushBack($2); $$ = $1; }
	;

external_declaration
	: function_definition
        { $$ = new std::variant<DeclNode*, FnDef*>($1); }
	| declaration
        { $$ = new std::variant<DeclNode*, FnDef*>($1); }
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement
		{ $$ = new FnDef($1, $2, $3); }
	;

declaration_list
	: declaration
	| declaration_list declaration
	;


%%

const TranslationUnit *g_root;

namespace AST
{
    const TranslationUnit *parseAST(const std::string &filename)
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