#pragma once

#include <memory>

#include <ast/models/function/ast_function_param_list.hpp>
#include <ast/models/declaration/ast_declarator.hpp>
#include <ast/models/ast_node_list.hpp>

namespace ast
{
    class FunctionDeclarator : public Declarator
    {
    public:
        FunctionDeclarator(const Declarator *declarator);

        FunctionDeclarator(
            const Declarator *declarator,
            const FunctionParamList *params);

        void print(std::ostream &dst, int indent_level) const override;

        std::vector<ir::Declaration> lower(Context &context) const override;

        std::string get_id() const override;

    private:
        std::unique_ptr<const Declarator> declarator;
        std::unique_ptr<const FunctionParamList> params;
    };
}
