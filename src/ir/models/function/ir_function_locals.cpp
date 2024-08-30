#include <ir/models/function/ir_function_locals.hpp>

namespace ir
{
    FunctionLocals::FunctionLocals(
        const std::vector<Declaration> locals,
        const std::vector<Constant> constants)
        : locals(locals),
          constants(constants)
    {
    }

    void FunctionLocals::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = get_indent(indent_level);
        dst << indent << "Locals:" << std::endl;
        for (const auto &local : locals)
        {
            local.print(dst, indent_level + 1);
        }
        dst << indent << "Constants:" << std::endl;
        for (const auto &constant : constants)
        {
            constant.print(dst, indent_level + 1);
        }
    }
}
