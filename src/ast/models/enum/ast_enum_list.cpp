#include <ast/models/enum/ast_enum_list.hpp>

namespace ast
{
    void EnumList::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        print_delim(dst, indent_level, ",\n" + indent);
    }
}
