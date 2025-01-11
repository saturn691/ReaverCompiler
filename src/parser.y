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
	void updateTypeDefs(std::string id);
}

%union {
    TranslationUnit                     *tu;
    FnDef                  				*func_def;
    DeclNode                     		*decl_node;
	
	std::variant<DeclNode*, Stmt*>		*block_item_;
	Assignment::Op                      assignment_op;
	ArgExprList                         *arg_expr_list;
	BlockItemList					   	*block_item_list_;
	CompoundStmt					   	*compound_stmt;
	CompoundTypeDecl				   	*compound_type;
	Enum						   		*enum_;
	EnumMemberList					   	*enum_member_list;
	EnumMember					   		*enum_member;
	ExprStmt					   		*expr_stmt;
	Init					   			*init;
	InitDecl					   		*init_decl;
	InitDeclList					   	*init_decl_list;
	InitList					   		*init_list;
	Struct::Type					   	struct_type;
	StructDecl					   		*struct_decl;
	StructDeclList					   	*struct_decl_list;
	StructMember					   	*struct_member;
	StructMemberList				   	*struct_member_list;
	ParamDecl					   		*param_decl;
	ParamList					   		*param_list;
	PtrNode					   			*ptr_node;
	UnaryOp::Op					   		unary_op;
	
	Decl                         		*decl;
	Expr 					   			*expr;
	Stmt 					   			*stmt;
	TypeDecl				  			*type;
    
	std::variant<DeclNode*, FnDef*>     *ext_decl;

    std::string                         *string;
    int                                 integer;
    yytokentype                         token;
}


%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN SIZEOF

%token TYPEDEF EXTERN STATIC AUTO REGISTER INLINE RESTRICT
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token BOOL COMPLEX IMAGINARY
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%token <string> IDENTIFIER CONSTANT STRING_LITERAL TYPE_NAME
%type <string> string_literal

%type <assignment_op> assignment_operator
%type <arg_expr_list> argument_expression_list
%type <block_item_> block_item
%type <block_item_list_> block_item_list
%type <compound_stmt> compound_statement
%type <compound_type> declaration_specifiers specifier_qualifier_list
%type <decl> declarator direct_declarator abstract_declarator 
%type <decl> direct_abstract_declarator
%type <decl_node> declaration
%type <enum_> enum_specifier
%type <enum_member> enumerator
%type <enum_member_list> enumerator_list
%type <ext_decl> external_declaration
%type <expr> constant_expression
%type <expr> primary_expression postfix_expression unary_expression
%type <expr> cast_expression multiplicative_expression additive_expression
%type <expr> shift_expression relational_expression equality_expression
%type <expr> and_expression exclusive_or_expression inclusive_or_expression
%type <expr> logical_and_expression logical_or_expression
%type <expr> conditional_expression assignment_expression expression
%type <expr_stmt> expression_statement
%type <func_def> function_definition
%type <init> initializer
%type <init_decl> init_declarator
%type <init_decl_list> init_declarator_list
%type <init_list> initializer_list
%type <param_decl> parameter_declaration
%type <param_list> parameter_list parameter_type_list
%type <ptr_node> pointer
%type <struct_decl> struct_declarator
%type <struct_decl_list> struct_declarator_list
%type <struct_member> struct_declaration
%type <struct_member_list> struct_declaration_list
%type <struct_type> struct_or_union
%type <type> type_specifier 
%type <type> type_name struct_or_union_specifier type_qualifier
%type <type> function_specifier storage_class_specifier
%type <unary_op> unary_operator
%type <stmt> statement labeled_statement jump_statement
%type <stmt> selection_statement iteration_statement
%type <tu> translation_unit


%start root
%%

root
	: translation_unit
		{ g_root = $1; }
	;

/* 
C99 6.4.5: "In translation phase 6, the multibyte character sequences 
specified by any sequence of character and wide string literals are 
concatenated into a single multibyte character sequence."
*/ 
string_literal
	: STRING_LITERAL
		{ $$ = new std::string(*$1); }
	| string_literal STRING_LITERAL
		/* "a""b" = "ab" (remove the 2 middle quotation marks*/
        { $$ = new std::string(
			$1->substr(0, $1->length() - 1) + 
			$2->substr(1, $2->length() - 1)); }
	;


primary_expression
	: IDENTIFIER
		{ $$ = new Identifier(std::string(*$1)); }
	| CONSTANT
		{ $$ = new Constant(std::string(*$1)); }
	| string_literal
		{ $$ = new StringLiteral(std::string(*$1)); }
	| '(' expression ')'
		{ $$ = new Paren($2); }
	;

postfix_expression
	: primary_expression
		{ $$ = $1; }
	| postfix_expression '[' expression ']'
		{ $$ = new ArrayAccess($1, $3); }
	| postfix_expression '(' ')'
		{ $$ = new FnCall($1); }
	| postfix_expression '(' argument_expression_list ')'
		{ $$ = new FnCall($1, $3); }
	| postfix_expression '.' IDENTIFIER
		{ $$ = new StructAccess($1, std::string(*$3)); }
	| postfix_expression PTR_OP IDENTIFIER
		{ $$ = new StructPtrAccess($1, std::string(*$3)); }
	| postfix_expression INC_OP
		{ $$ = new UnaryOp($1, UnaryOp::Op::POST_INC); }
	| postfix_expression DEC_OP
		{ $$ = new UnaryOp($1, UnaryOp::Op::POST_DEC); }
	| '(' type_name ')' '{' initializer_list '}'
	| '(' type_name ')' '{' initializer_list ',' '}'
	;

argument_expression_list
	: assignment_expression
		{ $$ = new ArgExprList($1); }
	| argument_expression_list ',' assignment_expression
		{ $1->pushBack($3); $$ = $1; }
	;

unary_expression
	: postfix_expression
		{ $$ = $1; }
	| INC_OP unary_expression
		{ $$ = new UnaryOp($2, UnaryOp::Op::PRE_INC); }
	| DEC_OP unary_expression
		{ $$ = new UnaryOp($2, UnaryOp::Op::PRE_DEC); }
	| unary_operator cast_expression
		{ $$ = new UnaryOp($2, $1); }
	| SIZEOF unary_expression
		{ $$ = new SizeOf($2); }
	| SIZEOF '(' type_name ')'
		{ $$ = new SizeOf($3); }
	;

unary_operator
	: '&'
		{ $$ = UnaryOp::Op::ADDR; }
	| '*'
		{ $$ = UnaryOp::Op::DEREF; }
	| '+'
		{ $$ = UnaryOp::Op::PLUS; }
	| '-'
		{ $$ = UnaryOp::Op::MINUS; }
	| '~'
		{ $$ = UnaryOp::Op::NOT; }
	| '!'
		{ $$ = UnaryOp::Op::LNOT; }
	;

cast_expression
	: unary_expression
		{ $$ = $1; }
	| '(' type_name ')' cast_expression
		{ $$ = new Cast($2, $4); }
	;

multiplicative_expression
	: cast_expression
	 	{ $$ = $1; }
	| multiplicative_expression '*' cast_expression
		{ $$ = new BinaryOp($1, $3, BinaryOp::Op::MUL); }
	| multiplicative_expression '/' cast_expression
		{ $$ = new BinaryOp($1, $3, BinaryOp::Op::DIV); }
	| multiplicative_expression '%' cast_expression
		{ $$ = new BinaryOp($1, $3, BinaryOp::Op::MOD); }
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
		{ $$ = new BinaryOp($1, $3, BinaryOp::Op::SHL); }
	| shift_expression RIGHT_OP additive_expression
		{ $$ = new BinaryOp($1, $3, BinaryOp::Op::SHR); }
	;

relational_expression
	: shift_expression
		{ $$ = $1; }
	| relational_expression '<' shift_expression
		{ $$ = new BinaryOp($1, $3, BinaryOp::Op::LT); }
	| relational_expression '>' shift_expression
		{ $$ = new BinaryOp($1, $3, BinaryOp::Op::GT); }
	| relational_expression LE_OP shift_expression
		{ $$ = new BinaryOp($1, $3, BinaryOp::Op::LE); }
	| relational_expression GE_OP shift_expression
		{ $$ = new BinaryOp($1, $3, BinaryOp::Op::GE); }
	;

equality_expression
	: relational_expression
		{ $$ = $1; }
	| equality_expression EQ_OP relational_expression
		{ $$ = new BinaryOp($1, $3, BinaryOp::Op::EQ); }
	| equality_expression NE_OP relational_expression
		{ $$ = new BinaryOp($1, $3, BinaryOp::Op::NE); }
	;

and_expression
	: equality_expression
		{ $$ = $1; }
	| and_expression '&' equality_expression
		{ $$ = new BinaryOp($1, $3, BinaryOp::Op::AND); }
	;

exclusive_or_expression
	: and_expression
		{ $$ = $1; }
	| exclusive_or_expression '^' and_expression
		{ $$ = new BinaryOp($1, $3, BinaryOp::Op::XOR); }
	;

inclusive_or_expression
	: exclusive_or_expression
		{ $$ = $1; }
	| inclusive_or_expression '|' exclusive_or_expression
		{ $$ = new BinaryOp($1, $3, BinaryOp::Op::OR); }
	;

logical_and_expression
	: inclusive_or_expression
		{ $$ = $1; }
	| logical_and_expression AND_OP inclusive_or_expression
		{ $$ = new BinaryOp($1, $3, BinaryOp::Op::LAND); }
	;

logical_or_expression
	: logical_and_expression
		{ $$ = $1; }
	| logical_or_expression OR_OP logical_and_expression
		{ $$ = new BinaryOp($1, $3, BinaryOp::Op::LOR); }
	;

conditional_expression
	: logical_or_expression
		{ $$ = $1; }
	| logical_or_expression '?' expression ':' conditional_expression
		{ $$ = new TernaryOp($1, $3, $5); }
	;

assignment_expression
	: conditional_expression
		{ $$ = $1; }
	| unary_expression assignment_operator assignment_expression
		{ $$ = new Assignment($1, $3, $2); }
	;

assignment_operator
	: '='
		{ $$ = Assignment::Op::ASSIGN; }
	| MUL_ASSIGN
		{ $$ = Assignment::Op::MUL_ASSIGN; }
	| DIV_ASSIGN
		{ $$ = Assignment::Op::DIV_ASSIGN; }
	| MOD_ASSIGN
		{ $$ = Assignment::Op::MOD_ASSIGN; }
	| ADD_ASSIGN
		{ $$ = Assignment::Op::ADD_ASSIGN; }
	| SUB_ASSIGN
		{ $$ = Assignment::Op::SUB_ASSIGN; }
	| LEFT_ASSIGN
		{ $$ = Assignment::Op::LEFT_ASSIGN; }
	| RIGHT_ASSIGN
		{ $$ = Assignment::Op::RIGHT_ASSIGN; }
	| AND_ASSIGN
		{ $$ = Assignment::Op::AND_ASSIGN; }
	| XOR_ASSIGN
		{ $$ = Assignment::Op::XOR_ASSIGN; }
	| OR_ASSIGN
		{ $$ = Assignment::Op::OR_ASSIGN; }
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
		{ $$ = new DeclNode($1); }
	| TYPEDEF declaration_specifiers init_declarator_list ';'
		{
			for (const auto &node : $3->nodes_)
			{
				auto *decl = std::get<0>(node).get();
				updateTypeDefs(decl->getID());
			}

			$$ = new DeclNode(new Typedef($2), $3);
		}
	| declaration_specifiers init_declarator_list ';'
		{ $$ = new DeclNode($1, $2); }
	;

declaration_specifiers
	: storage_class_specifier
		{ $$ = new CompoundTypeDecl($1); }
	| storage_class_specifier declaration_specifiers
		{ $2->pushBack($1); $$ = $2; }
	| type_specifier
		{ $$ = new CompoundTypeDecl($1); }
	| type_specifier declaration_specifiers
		{ $2->pushBack($1); $$ = $2; }
	| type_qualifier
		{ $$ = new CompoundTypeDecl($1); }
	| type_qualifier declaration_specifiers
		{ $2->pushBack($1); $$ = $2; }
	| function_specifier
		{ $$ = new CompoundTypeDecl($1); }
	| function_specifier declaration_specifiers
		{ $2->pushBack($1); $$ = $2; }
	;

init_declarator_list
	: init_declarator
		{ $$ = new InitDeclList($1); }
	| init_declarator_list ',' init_declarator
		{ $1->pushBack($3); $$ = $1; }
	;

init_declarator
	: declarator
		{ $$ = new InitDecl($1); }
	| declarator '=' initializer
		{ $$ = new InitDecl($1, $3); }
	;

storage_class_specifier
	: EXTERN
		{ $$ = new TypeModifier(TypeModifier::StorageClass::EXTERN); }
	| STATIC
		{ $$ = new TypeModifier(TypeModifier::StorageClass::STATIC); }
	| AUTO
		{ $$ = new TypeModifier(TypeModifier::StorageClass::AUTO); }
	| REGISTER
		{ $$ = new TypeModifier(TypeModifier::StorageClass::REGISTER); }
	;

type_specifier
	: VOID
		{ $$ = new BasicTypeDecl(Types::VOID); }
	| CHAR
		{ $$ = new BasicTypeDecl(Types::CHAR); }
	| SHORT
		{ $$ = new BasicTypeDecl(Types::SHORT); }
	| INT
		{ $$ = new BasicTypeDecl(Types::INT); }
	| LONG
		{ $$ = new TypeModifier(TypeModifier::Length::LONG); }
	| FLOAT
		{ $$ = new BasicTypeDecl(Types::FLOAT); }
	| DOUBLE
		{ $$ = new BasicTypeDecl(Types::DOUBLE); }
	| SIGNED
		{ $$ = new TypeModifier(TypeModifier::Signedness::SIGNED); }
	| UNSIGNED
		{ $$ = new TypeModifier(TypeModifier::Signedness::UNSIGNED); }
	| BOOL
		{ $$ = new BasicTypeDecl(Types::BOOL); }
	| COMPLEX
		{ $$ = new TypeModifier(TypeModifier::Complex::COMPLEX); }
	| IMAGINARY
		{ $$ = new TypeModifier(TypeModifier::Complex::IMAGINARY); }
	| struct_or_union_specifier
		{ $$ = $1; }
	| enum_specifier
		{ $$ = $1; }
	| TYPE_NAME
		{ $$ = new DefinedTypeDecl(std::string(*$1)); }
	;

struct_or_union_specifier
	: struct_or_union IDENTIFIER '{' struct_declaration_list '}'
		{ $$ = new Struct($1, std::string(*$2), $4); }
	| struct_or_union '{' struct_declaration_list '}'
		{ $$ = new Struct($1, $3); }
	| struct_or_union IDENTIFIER
		{ $$ = new Struct($1, std::string(*$2)); }
	;

struct_or_union
	: STRUCT
		{ $$ = Struct::Type::STRUCT; }
	| UNION
		{ $$ = Struct::Type::UNION; }
	;

struct_declaration_list
	: struct_declaration
		{ $$ = new StructMemberList($1); }
	| struct_declaration_list struct_declaration
		{ $1->pushBack($2); $$ = $1; }
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'
		{ $$ = new StructMember($1, $2); }
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
		{ $2->pushBack($1); $$ = $2; }
	| type_specifier
		{ $$ = new CompoundTypeDecl($1); }
	| type_qualifier specifier_qualifier_list
		{ $2->pushBack($1); $$ = $2; }
	| type_qualifier
		{ $$ = new CompoundTypeDecl($1); }
	;

struct_declarator_list
	: struct_declarator
		{ $$ = new StructDeclList($1); }
	| struct_declarator_list ',' struct_declarator
		{ $1->pushBack($3); $$ = $1; }
	;

struct_declarator
	: declarator
		{ $$ = new StructDecl($1); }
	| ':' constant_expression
		/* Ignore bit fields for now */
	| declarator ':' constant_expression
	;

enum_specifier
	: ENUM '{' enumerator_list '}'
		{ $$ = new Enum($3); }
	| ENUM IDENTIFIER '{' enumerator_list '}'
		{ $$ = new Enum(std::string(*$2), $4); }
	| ENUM '{' enumerator_list ',' '}'
		{ $$ = new Enum($3); }
	| ENUM IDENTIFIER '{' enumerator_list ',' '}'
		{ $$ = new Enum(std::string(*$2), $4); }
	| ENUM IDENTIFIER
	 	{ $$ = new Enum(std::string(*$2)); }
	;

enumerator_list
	: enumerator
		{ $$ = new EnumMemberList($1); }
	| enumerator_list ',' enumerator
		{ $$ = $1; $1->pushBack($3); }
	;

enumerator
	: IDENTIFIER
		{ $$ = new EnumMember(std::string(*$1)); }
	| IDENTIFIER '=' constant_expression
		{ $$ = new EnumMember(std::string(*$1), $3); }
	;

type_qualifier
	: CONST
		{ $$ = new TypeModifier(CVRQualifier::CONST); }
	| RESTRICT
		{ $$ = new TypeModifier(CVRQualifier::RESTRICT); }
	| VOLATILE
		{ $$ = new TypeModifier(CVRQualifier::VOLATILE); }
	;

function_specifier
	: INLINE
		{ $$ = new TypeModifier(FunctionSpecifier::INLINE); }
	;

declarator
	: pointer direct_declarator
		{ $$ = new PtrDecl($1, $2); }
	| direct_declarator
		{ $$ = $1; }
	;


direct_declarator
	: IDENTIFIER
		{ $$ = new Identifier(std::string(*$1)); }
	| '(' declarator ')'
		{ $$ = new Paren($2); }
	| direct_declarator '[' type_qualifier_list assignment_expression ']'
	| direct_declarator '[' type_qualifier_list ']'
	| direct_declarator '[' assignment_expression ']'
		{ $$ = new ArrayDecl($1, $3); }
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
		{ $$ = new PtrNode(); }
	| '*' type_qualifier_list
	| '*' pointer
		{ $$ = new PtrNode($2); }
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
		{ $$ = new ParamDecl(new AbstractTypeDecl($1, $2)); }
	| declaration_specifiers
		{ $$ = new ParamDecl($1); }
	;

identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER
	;

type_name
	: specifier_qualifier_list
		{ $$ = $1; }
	| specifier_qualifier_list abstract_declarator
		{ $$ = new AbstractTypeDecl($1, $2); }
	;

abstract_declarator
	: pointer
		{ $$ = $1; }
	| direct_abstract_declarator
		{ $$ = $1; }
	| pointer direct_abstract_declarator
		{ $$ = new PtrDecl($1, $2); }
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'
		{ $$ = new Paren($2); }
	| '[' ']'
	| '[' assignment_expression ']'
		{ $$ = new AbstractArrayDecl($2); }
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' assignment_expression ']'
		{ $$ = new AbstractArrayDecl($1, $3); }
	| '[' '*' ']'
	| direct_abstract_declarator '[' '*' ']'
	| '(' ')'
	| '(' parameter_type_list ')'
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_type_list ')'
	;

initializer
	: assignment_expression
		{ $$ = new Init($1); }
	| '{' initializer_list '}'
		{ $$ = new Init($2); }
	| '{' initializer_list ',' '}'
		{ $$ = new Init($2); }
	;

initializer_list
	: initializer
		{ $$ = new InitList($1); }
	| designation initializer
	| initializer_list ',' initializer
		{ $1->pushBack($3); $$ = $1; }
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
		{ $$ = new Case($2, $4); }
	| DEFAULT ':' statement
		{ $$ = new Case($3); }
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
		{ $$ = new ExprStmt(); }
	| expression ';'
		{ $$ = new ExprStmt($1); }
	;

selection_statement
	: IF '(' expression ')' statement
		/* Must be placed in this order, solves dangling else */
		{ $$ = new IfElse($3, $5); }
	| IF '(' expression ')' statement ELSE statement
		{ $$ = new IfElse($3, $5, $7); }
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
	| FOR '(' declaration expression_statement ')' statement
		{ $$ = new For($3, $4, $6); }
	| FOR '(' declaration expression_statement expression ')' statement
		{ $$ = new For($3, $4, $5, $7); }
	;

jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';'
		{ $$ = new Continue(); }
	| BREAK ';'
		{ $$ = new Break(); }
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