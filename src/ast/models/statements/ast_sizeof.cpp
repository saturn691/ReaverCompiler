#include <ast/models/statements/ast_sizeof.hpp>

namespace ast
{
    Sizeof::Sizeof(
        const Type *type)
        : type(std::shared_ptr<const Type>(type))
    {
    }

    Sizeof::Sizeof(
        const Node *expression)
        : expression(std::shared_ptr<const Node>(expression))
    {
    }

    void Sizeof::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        dst << indent << "sizeof";
        if (type)
        {
            dst << "(";
            type->print(dst, 0);
            dst << ")";
        }
        else
        {
            expression->print(dst, 0);
        }
    }

    void Sizeof::lower(Context &context) const
    {
    }
}
