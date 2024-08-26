#pragma once

#include <memory>

#include <ast/models/declaration/ast_declaration.hpp>
#include <ast/models/statements/ast_statement.hpp>
#include <ast/models/ast_node.hpp>
#include <ast/models/ast_node_list.hpp>

namespace ast
{
    class Scope : public Node
    {
    public:
        using StatementList = NodeList<Statement>;
        using TranslationUnit = NodeList<Declaration>;

        Scope();

        Scope(const TranslationUnit *declarations);

        Scope(const TranslationUnit *declarations,
              const StatementList *statements);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const;

    private:
        std::unique_ptr<const TranslationUnit> declarations;
        std::unique_ptr<const StatementList> statements;
    };
}
