#include <ast/models/array/ast_array_access.hpp>

namespace ast
{
    ArrayAccess::ArrayAccess(
        const Node *array,
        const Node *expr)
        : array(std::unique_ptr<const Node>(array)),
          expr(std::unique_ptr<const Node>(expr))
    {
    }

    void ArrayAccess::print(std::ostream &dst, int indent_level) const
    {
        dst << Utils::get_indent(indent_level);
        array->print(dst, 0);
        dst << "[";
        expr->print(dst, 0);
        dst << "]";
    }

    void ArrayAccess::lower(Context &context) const
    {
    }
}
