#include <ir/models/ir_ir.hpp>

namespace ir
{
    void IR::print(std::ostream &dst, int indent_level) const
    {
    }

    void IR::add_function(const Function &function)
    {
        functions.push_back(function);
    }

    void IR::add_declaration(const Declaration &declaration)
    {
        declarations.push_back(declaration);
    }
}
