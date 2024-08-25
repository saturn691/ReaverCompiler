#include <ast/models/struct/ast_struct.hpp>

namespace ast
{
    Struct::Struct(
        const StructOrUnionType type,
        const std::string identifier)
        : type(type),
          identifier(identifier)
    {
    }

    void Struct::print(std::ostream &dst, int indent_level) const
    {
        dst << Utils::get_indent(indent_level);

        switch (type)
        {
        case StructOrUnionType::STRUCT:
            dst << "struct";
            break;
        case StructOrUnionType::UNION:
            dst << "union";
            break;
        }

        dst << " " << identifier;
    }

    unsigned int Struct::get_size() const
    {
        return 0;
    }
}
