#include <ast/models/function/ast_function_param_list.hpp>

namespace ast
{
    void FunctionParamList::print(std::ostream &dst, int indent_level) const
    {
        print_delim(dst, indent_level, ", ");
    }

    std::vector<ir::Declaration>
    FunctionParamList::lower(Context &context) const
    {
        using Node = std::unique_ptr<const ast::FunctionParam>;

        std::vector<ir::Declaration> declarations;
        for (const auto &node : nodes)
        {
            ir::Declaration decl = std::get<Node>(node)->lower(context);
            declarations.push_back(decl);
        }

        return declarations;
    }
}
