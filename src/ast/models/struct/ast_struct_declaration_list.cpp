#include <ast/models/struct/ast_struct_declaration_list.hpp>

namespace ast
{
    void StructDeclarationList::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        print_delim(dst, indent_level, "\n" + indent);
    }
}
