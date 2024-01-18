#ifndef ast_pointer_declarator_hpp
#define ast_pointer_declarator_hpp

#include "../ast_node.hpp"

/*
 *  Node for pointer declaration (e.g. "int* x;")
*/
class PointerDeclarator : public Node
{
public:
    PointerDeclarator(
        NodePtr _direct_declarator
    ) :
        direct_declarator(_direct_declarator)
    {}

    virtual ~PointerDeclarator()
    {
        delete direct_declarator;
    }

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent(AST_PRINT_INDENT_SPACES * indent_level, ' ');

        dst << indent;
        dst << "*";
        direct_declarator->print(dst, 0); // x
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("PointerDeclarator::evaluate() not implemented");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        throw std::runtime_error("PointerDeclarator::gen_asm() not implemented");

        // std::string indent(AST_PRINT_INDENT_SPACES, ' ');

        // direct_declarator->gen_asm(dst, dest_reg, context);

        // Types type = direct_declarator->get_type(context);
        // std::string id = direct_declarator->get_id();
        // int stack_loc = context.allocate_pointer_stack(type, id);

        // std::string ptr_reg = context.allocate_register(type);

        // switch (type)
        // {
        //     case Types::INT:
        //     case Types::UNSIGNED_INT:
        //         dst << indent << "sw " << ptr_reg << ", "
        //             << stack_loc << "(s0)" << std::endl;
        //         break;

        //     case Types::FLOAT:
        //         dst << indent << "fsw " << ptr_reg << ", "
        //             << stack_loc << "(s0)" << std::endl;
        //         break;

        //     case Types::DOUBLE:
        //         dst << indent << "dsw " << ptr_reg << ", "
        //             << stack_loc << "(s0)" << std::endl;
        //         break;

        //     default:
        //         throw std::runtime_error(
        //             "PointerDeclarator::gen_asm(): Unsupported type for assignment"
        //         );
        //         break;
        // }

        // context.deallocate_register(ptr_reg);
    }

private:
    // * direct_declarator
    // * x
    NodePtr direct_declarator;
};

#endif // ast_pointer_declarator_hpp
