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

    ir::Type BasicType::lower(Context &context) const
    {
        ir::Types t;

        switch (type)
        {
        case Types::VOID:
            t = ir::Types::VOID;
            break;
        case Types::UNSIGNED_CHAR:
            t = ir::Types::UNSIGNED_CHAR;
            break;
        case Types::CHAR:
            t = ir::Types::CHAR;
            break;
        case Types::UNSIGNED_SHORT:
            t = ir::Types::UNSIGNED_SHORT;
            break;
        case Types::SHORT:
            t = ir::Types::SHORT;
            break;
        case Types::UNSIGNED_INT:
            t = ir::Types::UNSIGNED_INT;
            break;
        case Types::INT:
            t = ir::Types::INT;
            break;
        case Types::UNSIGNED_LONG:
            t = ir::Types::UNSIGNED_LONG;
            break;
        case Types::LONG:
            t = ir::Types::LONG;
            break;
        case Types::FLOAT:
            t = ir::Types::FLOAT;
            break;
        case Types::DOUBLE:
            t = ir::Types::DOUBLE;
            break;
        case Types::LONG_DOUBLE:
            t = ir::Types::LONG_DOUBLE;
            break;
        }

        return ir::Type(t);
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
