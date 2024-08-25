#include <ast/models/type/ast_basic_type.hpp>

namespace ast
{
    BasicType::BasicType(const Types type)
        : type(type)
    {
    }

    unsigned int BasicType::get_size() const
    {
        switch (type)
        {
        case Types::VOID:
            return 0;
        case Types::UNSIGNED_CHAR:
            return 1;
        case Types::CHAR:
            return 1;
        case Types::UNSIGNED_SHORT:
            return 2;
        case Types::SHORT:
            return 2;
        case Types::UNSIGNED_INT:
            return 4;
        case Types::INT:
            return 4;
        case Types::UNSIGNED_LONG:
            return 4;
        case Types::LONG:
            return 4;
        case Types::FLOAT:
            return 4;
        case Types::DOUBLE:
            return 8;
        case Types::LONG_DOUBLE:
            return 8;
        }

        return 0;
    }

    void BasicType::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = Utils::get_indent(indent_level);
        dst << indent;

        switch (type)
        {
        case Types::VOID:
            dst << "void";
            break;
        case Types::UNSIGNED_CHAR:
            dst << "unsigned char";
            break;
        case Types::CHAR:
            dst << "char";
            break;
        case Types::UNSIGNED_SHORT:
            dst << "unsigned short";
            break;
        case Types::SHORT:
            dst << "short";
            break;
        case Types::UNSIGNED_INT:
            dst << "unsigned int";
            break;
        case Types::INT:
            dst << "int";
            break;
        case Types::UNSIGNED_LONG:
            dst << "unsigned long";
            break;
        case Types::LONG:
            dst << "long";
            break;
        case Types::FLOAT:
            dst << "float";
            break;
        case Types::DOUBLE:
            dst << "double";
            break;
        case Types::LONG_DOUBLE:
            dst << "long double";
            break;
        }
    }
}
