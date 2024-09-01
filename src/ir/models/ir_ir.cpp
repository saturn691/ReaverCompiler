#include <ir/models/ir_ir.hpp>

namespace ir
{
    void IR::print(std::ostream &dst, int indent_level) const
    {
        for (const auto &declaration : declarations)
        {
            declaration->print(dst, indent_level);
            dst << std::endl;
        }

        for (const auto &function : functions)
        {
            function->print(dst, indent_level);
            dst << std::endl;
        }
    }

    void IR::add_function(std::unique_ptr<Function> function)
    {
        functions.push_back(std::move(function));
    }

    void IR::add_declaration(std::unique_ptr<Declaration> declaration)
    {
        declarations.push_back(std::move(declaration));
    }
}
