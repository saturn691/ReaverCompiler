#include <ir/models/ir_function.hpp>

namespace ir
{
    Function::Function(
        const std::string name,
        const Declaration ret,
        const std::vector<Declaration> params)
        : name(name),
          ret(ret),
          params(params)
    {
    }

    Function::Function(const Function &other)
        : name(other.name),
          ret(other.ret),
          params(other.params)
    {
    }

    void Function::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = get_indent(indent_level);
        dst << indent << "Function: " << name << std::endl;

        dst << indent << "Return: ";
        ret.print(dst, indent_level + 1);

        dst << indent << "Params: " << std::endl;
        for (const auto &param : params)
        {
            param.print(dst, indent_level + 1);
        }
    }
}
