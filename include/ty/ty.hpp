#pragma once

#include <iostream>

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

    enum class Sign
    {
        UNSIGNED = 0,
        SIGNED = 1,
    };

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
    std::string to_string(Types type);
}
