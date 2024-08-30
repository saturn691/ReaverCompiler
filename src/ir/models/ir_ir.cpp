#include <ir/models/ir_ir.hpp>

namespace ir
{
    void IR::print(std::ostream &dst, int indent_level) const
    {
        for (const auto &declaration : declarations)
        {
            declaration.print(dst, indent_level);
            dst << std::endl;
        }

        for (const auto &function : functions)
        {
            function.print(dst, indent_level);
            dst << std::endl;
        }
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
