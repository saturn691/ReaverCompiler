#include <ty/ty.hpp>

#include <stdexcept>
#include <string>

namespace ty
{
/**
 * We choose FunctionType, as exponention is RIGHT associative.
 * Some may even call this 'currying'
 */
CompoundType
to_function_type(CompoundType return_type, std::vector<CompoundType> arguments)
{
    return std::make_shared<FunctionType>(FunctionType{return_type, arguments});
}

CompoundType from_function_type(CompoundType ty)
{
    if (auto ptr = std::get_if<std::shared_ptr<FunctionType>>(&ty))
    {
        return (*ptr)->return_type;
    }
    throw std::runtime_error("from_function_type() errors");
}

CompoundType to_ptr_type(CompoundType ty)
{
    return std::make_shared<PtrType>(PtrType{ty});
}

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

std::string to_string(CompoundType type)
{
    auto ty = std::get<Types>(type);
    switch (ty)
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
