#include <ast/models/primitives/ast_identifier.hpp>

namespace ast
{
    Identifier::Identifier(std::string id)
        : id(id)
    {
    }

    void Identifier::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        dst << indent << id;
    }

    std::vector<ir::Declaration> Identifier::lower(Context &context) const
    {
    }

    std::string Identifier::get_id() const
    {
        return id;
    }
}
