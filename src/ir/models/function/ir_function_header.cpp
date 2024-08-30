#include <ir/models/function/ir_function_header.hpp>

namespace ir
{
    FunctionHeader::FunctionHeader(
        const std::string name,
        const Declaration ret,
        const std::vector<Declaration> params)
        : name(name),
          ret(ret),
          params(params)
    {
    }

    void FunctionHeader::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = get_indent(indent_level);
        dst << indent << "function " << name << "(";
        for (size_t i = 0; i < params.size(); i++)
        {
            params[i].print(dst, 0);
            if (i < params.size() - 1)
            {
                dst << ", ";
            }
        }
        dst << ") -> ";
        ret.print(dst, 0);
    }
}
