#pragma once

#include <variant>

#include <ast/models/ast_declaration.hpp>
#include <ast/models/ast_node_list.hpp>

#include <ir/models/ir_ir.hpp>

namespace ast
{
    /**
     * Top level list for all declarations in the whole program.
     * Root node for the AST.
     */
    class TranslationUnit : public NodeList<DeclarationNode, FunctionDefinition>
    {
    public:
        using NodeList::NodeList;

        void print(std::ostream &dst, int indent_level) const override;

        std::unique_ptr<ir::IR> lower(Context &context) const;
    };
}
