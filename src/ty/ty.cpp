#include <ty/ty.hpp>

namespace ty
{

    /**
     * Subject to the following rules:
     * C99 6.3.1.1 Boolean, characters, and integers
     *
     * If an int can represent all values of the original type, the value is
     * converted to an int; otherwise, it is converted to an unsigned int.
     * These are called the integer promotions
     */
    Types integer_promotion(Types type)
    {

        switch (type)
        {
        case Types::_BOOL:
        case Types::CHAR:
        case Types::SHORT:
        case Types::UNSIGNED_CHAR:
        case Types::UNSIGNED_SHORT:
            return Types::INT;
        default:
            return type;
        }
    }

    /**
     * Subject to the following rules:
     * C99 6.3.1.8 Usual arithmetic conversions
     */
    Types promote(Types lhs, Types rhs)
    {
        if (lhs == Types::LONG_DOUBLE || rhs == Types::LONG_DOUBLE)
        {
            return Types::LONG_DOUBLE;
        }
        if (lhs == Types::DOUBLE || rhs == Types::DOUBLE)
        {
            return Types::DOUBLE;
        }
        if (lhs == Types::FLOAT || rhs == Types::FLOAT)
        {
            return Types::FLOAT;
        }

        // Integer promotions are performed on both operands.
        lhs = integer_promotion(lhs);
        rhs = integer_promotion(rhs);

        // Same type -- no conversion is needed.
        if (lhs == rhs)
        {
            return lhs;
        }
        // Same signedness - convert to the operand with greater rank.
        // Mixed signedness - convert to the operand with greater rank.
        if (get_rank(lhs) != get_rank(rhs))
        {
            return get_rank(lhs) > get_rank(rhs) ? lhs : rhs;
        }

        // Same rank - convert to the unsigned type.
        return get_sign(lhs) == Sign::UNSIGNED ? lhs : rhs;
    }

    Sign get_sign(Types type)
    {
        switch (type)
        {
        case Types::VOID:
        case Types::_BOOL:
        case Types::UNSIGNED_CHAR:
        case Types::UNSIGNED_SHORT:
        case Types::UNSIGNED_INT:
        case Types::UNSIGNED_LONG:
            return Sign::UNSIGNED;

        case Types::CHAR:
        case Types::SHORT:
        case Types::INT:
        case Types::LONG:
        case Types::FLOAT:
        case Types::DOUBLE:
        case Types::LONG_DOUBLE:
            return Sign::SIGNED;
        }
    }

    int get_rank(Types type)
    {
        // See C99 6.3.1.1

        switch (type)
        {
        case Types::VOID:
            return 0;
        case Types::_BOOL:
            return 1;
        case Types::CHAR:
        case Types::UNSIGNED_CHAR:
            return 2;
        case Types::SHORT:
        case Types::UNSIGNED_SHORT:
            return 3;
        case Types::INT:
        case Types::UNSIGNED_INT:
            return 4;
        case Types::LONG:
        case Types::UNSIGNED_LONG:
            return 5;
        case Types::FLOAT:
        case Types::DOUBLE:
        case Types::LONG_DOUBLE:
            // Not really true, but whatever
            return 6;
        }
    }

    std::string to_string(Types type)
    {
        switch (type)
        {
        case Types::VOID:
            return "void";
        case Types::_BOOL:
            return "_Bool";
        case Types::UNSIGNED_CHAR:
            return "unsigned char";
        case Types::CHAR:
            return "char";
        case Types::UNSIGNED_SHORT:
            return "unsigned short";
        case Types::SHORT:
            return "short";
        case Types::UNSIGNED_INT:
            return "unsigned int";
        case Types::INT:
            return "int";
        case Types::UNSIGNED_LONG:
            return "unsigned long";
        case Types::LONG:
            return "long";
        case Types::FLOAT:
            return "float";
        case Types::DOUBLE:
            return "double";
        case Types::LONG_DOUBLE:
            return "long double";
        }
    }
} // namespace ty
