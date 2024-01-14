#ifndef ast_types_hpp
#define ast_types_hpp


/*
 *  Types available in C/C++.
 *  Note that unsigned floats, doubles and long doubles do not exist.
 *  Sorted by priority. Higher value = more priority.
*/
enum class Types
{
    VOID,
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
    LONG_DOUBLE,
    STRUCT
};

#endif  /* ast_types_hpp */
