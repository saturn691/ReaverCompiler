#ifndef ast_function_definition_hpp
#define ast_function_definition_hpp

#include "../ast_node.hpp"
#include "../ast_declarator.hpp"

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
        Node* _compound_statement
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
        // Ok, if you're reading this, please don't question this line
        // This is to prevent declarations from generating assembly
        std::string code = "MAGIC CODE";

        // Let the children nodes know of the return type
        context.current_declaration_type = (TypePtr)declaration_specifier;
        Types type = context.current_declaration_type->get_type();
        std::string return_reg = "";

        std::string end_label = id + "_end";
        context.end_label_stack.push(end_label);

        switch (type)
        {
            case Types::FLOAT:
            case Types::DOUBLE:
            case Types::LONG_DOUBLE:
                return_reg = "fa0";
                break;

            default:
                return_reg = "a0";
                break;
        }

        // Header section
        dst << ".text" << std::endl;
        dst << ".globl " << id << std::endl;
        dst << std::endl;

        // Body section
        declarator->gen_asm(dst, code, context);
        // context.init_stack is called in the declarator
        compound_statement->gen_asm(dst, return_reg, context);

        // Return nodes jump here
        dst << end_label << ":" << std::endl;
        context.end_stack(dst);
        dst << AST_INDENT << "ret" << std::endl;

        // Footer section
        std::cout << std::endl;
        context.gen_memory_asm(dst);
    }

private:
    Type* declaration_specifier;
    Declarator* declarator;
    Node* compound_statement;
};


#endif  /* ast_function_definition_hpp */
