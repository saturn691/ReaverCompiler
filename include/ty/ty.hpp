#pragma once

#include <map>
#include <memory>
#include <variant>
#include <vector>

/**
 * Module for all types across the compiler.
 */

namespace ty
{
/**
 *  Types available in C/C++.
 *  Note that unsigned floats, doubles and long doubles do not exist.
 *  Sorted by priority. Higher value = more priority.
 */
enum class Types
{
    VOID,
    _BOOL, // C99 feature
    UNSIGNED_CHAR,
    CHAR,
    UNSIGNED_SHORT,
    SHORT,
    UNSIGNED_INT,
    INT,
    UNSIGNED_LONG,
    LONG,
    FLOAT,
    DOUBLE,
    LONG_DOUBLE
};

struct UnionType;
struct PtrType;
struct ArrayType;
struct StructType;
struct EnumType;
struct FunctionType;

using CompoundType = std::variant<
    Types,
    std::shared_ptr<UnionType>,
    std::shared_ptr<PtrType>,
    std::shared_ptr<StructType>,
    std::shared_ptr<EnumType>,
    std::shared_ptr<FunctionType>>;

// Left associative, used for structs
struct UnionType
{
    CompoundType lhs;
    CompoundType rhs;
};

// You could argue it's an ExpType, but with what?
struct PtrType
{
    CompoundType lhs;
};

// Includes VLAs
struct ArrayType
{
    CompoundType lhs;
    // Specify 0 for VLA or unknown size
    size_t size;
};

struct StructType
{
    CompoundType lhs;
    CompoundType rhs;
};

struct EnumType
{
    std::map<std::string, int> values;
};

struct FunctionType
{
    CompoundType return_type;
    std::vector<CompoundType> arguments;
};

enum class Sign
{
    UNSIGNED = 0,
    SIGNED = 1,
};

/**
 * Converts a C function to its ADT
 */
CompoundType
to_function_type(CompoundType return_type, std::vector<CompoundType> arguments);

/**
 * Gets the return type of a function type
 */
CompoundType from_function_type(CompoundType ty);

/**
 * Converts a type to a pointer type
 */
CompoundType to_ptr_type(CompoundType ty);

/**
 * Uses the C99 rules for integer promotion.
 */
Types integer_promotion(Types type);

/**
 * Promotes two types to a common type.
 *
 * Subject to the following rules:
 * C99 6.3.1.8 Usual arithmetic conversions
 */
Types promote(Types lhs, Types rhs);

/**
 * Checks if a type is signed.
 */
Sign get_sign(Types type);

/**
 * Gets the integer rank of a type.
 */
int get_rank(Types type);

/**
 * Converts a type to a string.
 */
std::string to_string(CompoundType type);
} // namespace ty
