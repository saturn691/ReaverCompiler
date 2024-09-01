#include <ast/models/ast_translation_unit.hpp>
#include <ast/models/ast_declaration.hpp>

#include <ir/models/ir_ir.hpp>

namespace ast
{
    void TranslationUnit::print(std::ostream &dst, int indent_level) const
    {
        print_delim(dst, indent_level, "\n");
    }

    std::unique_ptr<ir::IR> TranslationUnit::lower(Context &context) const
    {
        std::unique_ptr<ir::IR> ir = std::make_unique<ir::IR>();
        for (const auto &node : nodes)
        {
            std::visit(
                [&context, &ir](const auto &n)
                {
                    n->lower(context, ir);
                },
                node);
        }

        return ir;
    }
}
