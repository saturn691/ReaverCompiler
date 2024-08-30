#include <ir/models/ir_type.hpp>

namespace ir
{
    Type::Type(const Types type)
        : type(type)
    {
    }

    void Type::print(std::ostream &dst, int indent_level) const
    {
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
