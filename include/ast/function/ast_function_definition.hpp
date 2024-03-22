#ifndef ast_function_definition_hpp
#define ast_function_definition_hpp

#include "../ast_node.hpp"
#include "../ast_declarator.hpp"
#include "../scopes/ast_scope_manager.hpp"

/*
 *  Node for defining functions (e.g. "int f() { return 5; }")
*/
class FunctionDefinition : public Node
{
public:
    // No arguments provided into the function definition
    FunctionDefinition(
        Type* _declaration_specifier,
        Declarator* _declarator,
        ScopeManager* _compound_statement
    ) :
        declaration_specifier(_declaration_specifier),
        declarator(_declarator),
        compound_statement(_compound_statement)
    {}

    virtual ~FunctionDefinition()
    {
        delete declaration_specifier;
        delete declarator;
        delete compound_statement;
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        declaration_specifier->print(dst, 0);
        dst << " ";
        declarator->print(dst, 0);
        dst << std::endl;

        compound_statement->print(dst, indent_level + 1);
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string id = declarator->get_id();
        context.mode_stack.push(Context::Mode::FUNCTION_DEFINITION);

        context.reset_registers();
        context.reset_frame_pointer();
        context.reset_stack_pointer();

        // Let the children nodes know of the return type
        context.current_declaration_type = (TypePtr)declaration_specifier;
        Types type = context.current_declaration_type->get_type();

        std::string return_reg = context.allocate_return_register(type);

        std::string end_label = id + "_end";
        context.end_label_stack.push(end_label);

        // Header section
        dst << AST_INDENT << ".text" << std::endl;
        dst << AST_INDENT << ".globl " << id << std::endl;
        dst << AST_INDENT << ".type " << id << ", @function" << std::endl;
        dst << std::endl;

        // Body section (ignore the register, it's not used here)
        declarator->gen_asm(dst, return_reg, context);
        // context.init_stack is called in the declarator
        compound_statement->gen_asm(dst, return_reg, context);

        // Return nodes jump here
        dst << end_label << ":" << std::endl;
        context.end_stack(dst);
        dst << AST_INDENT << "ret" << std::endl;

        // Footer section
        std::cout << std::endl;
        context.gen_memory_asm(dst);

        // Lookout for global variables
        context.mode_stack.pop();
    }

private:
    Type* declaration_specifier;
    Declarator* declarator;
    ScopeManager* compound_statement;
};


#endif  /* ast_function_definition_hpp */
