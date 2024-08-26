#include <ast/models/declaration/ast_translation_unit.hpp>

namespace ast
{
    void TranslationUnit::print(std::ostream &dst, int indent_level) const
    {
        print_delim(dst, indent_level, "\n");
    }

    ir::IR TranslationUnit::lower(Context &context) const
    {
        ir::IR ir = ir::IR();
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
