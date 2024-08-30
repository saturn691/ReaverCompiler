#include <ir/models/function/ir_function.hpp>

namespace ir
{
    Function::Function(
        const FunctionHeader header,
        const FunctionLocals locals,
        const std::vector<BasicBlock> bbs)
        : header(header),
          locals(locals),
          bbs(bbs)
    {
    }

    void Function::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = get_indent(indent_level);
        header.print(dst, indent_level);
        dst << std::endl
            << indent << "{" << std::endl;

        locals.print(dst, indent_level + 1);

        for (const auto &bb : bbs)
        {
            bb.print(dst, indent_level + 1);
            dst << std::endl;
        }

        dst << indent << "}" << std::endl;
    }
}
