#include <ir/models/ir_function.hpp>

namespace ir
{
    /*************************************************************************
     * FunctionHeader implementation
     ************************************************************************/

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

    /*************************************************************************
     * FunctionLocals implementation
     ************************************************************************/

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

    /*************************************************************************
     * Function implementation
     ************************************************************************/

    Function::Function(
        const FunctionHeader &header,
        const FunctionLocals &locals,
        BasicBlocks &&bbs)
        : header(header),
          locals(locals),
          bbs(std::move(bbs))
    {
    }

    void Function::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = get_indent(indent_level);
        header.print(dst, indent_level);
        dst << std::endl
            << indent << "{" << std::endl;

        locals.print(dst, indent_level + 1);
        dst << std::endl;

        for (size_t i = 0; i < bbs.size(); i++)
        {
            dst << get_indent(1) << "BasicBlock " << i << ":" << std::endl;
            bbs[i]->print(dst, indent_level + 1);
            dst << std::endl;
        }

        dst << indent << "}" << std::endl;
    }

}
