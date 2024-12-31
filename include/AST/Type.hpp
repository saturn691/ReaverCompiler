#pragma once

#include <iostream>
#include <memory>
#include <vector>

namespace AST
{
template <typename T>
using Ptr = std::unique_ptr<const T>;

// Forward declarations
class Decl;
class ParamType;
class StructDeclList;

/**
 * Base class for types.
 */
class BaseType
{
public:
    virtual ~BaseType() = default;
    virtual bool operator==(const BaseType &other) const = 0;
    virtual bool operator!=(const BaseType &other) const = 0;
    virtual bool operator<(const BaseType &other) const = 0;
    virtual bool operator<=(const BaseType &other) const = 0;
    virtual Ptr<BaseType> clone() const = 0;
};

template <typename Derived>
class Type : public virtual BaseType
{
public:
    virtual ~Type() = default;
    Ptr<BaseType> clone() const override
    {
        return cloneAsDerived();
    }

    Ptr<Derived> cloneAsDerived() const
    {
        return std::make_unique<Derived>(derived());
    }

    bool operator==(const BaseType &other) const override
    {
        if (auto otherType = dynamic_cast<const Derived *>(&other))
        {
            return derived() == *otherType;
        }

        return false;
    }

    bool operator!=(const BaseType &other) const override
    {
        return !(*this == other);
    }

    bool operator<=(const BaseType &other) const override
    {
        return *this == other || *this < other;
    }

    virtual bool operator==(const Derived &other) const = 0;

private:
    const Derived &derived() const
    {
        return static_cast<const Derived &>(*this);
    }

    Type() = default;
    Type(const Type &) = default;
    friend Derived;
};

/**
 * Standard types
 */
enum class Types
{
    VOID,
    BOOL,
    CHAR,
    UNSIGNED_CHAR,
    SHORT,
    UNSIGNED_SHORT,
    INT,
    UNSIGNED_INT,
    LONG,
    UNSIGNED_LONG,
    FLOAT,
    DOUBLE,
    LONG_DOUBLE,
    COMPLEX,
    IMAGINARY
};

class ArrayType final : public Type<ArrayType>
{
public:
    ArrayType(Ptr<BaseType> type, size_t size);
    ArrayType(const ArrayType &other);

    bool operator==(const ArrayType &other) const override;
    bool operator<(const BaseType &other) const override;

    Ptr<BaseType> type_;
    size_t size_;
};

/**
 * Basic types
 * e.g. `int`, `float`, `char`
 */
class BasicType final : public Type<BasicType>
{
public:
    BasicType(Types type);
    BasicType(const BasicType &other);

    bool operator==(const BasicType &other) const override;
    bool operator<(const BaseType &other) const override;

    Types type_;
};

using EnumConsts = std::vector<std::pair<std::string, int>>;

/**
 * Enum types
 */
class EnumType final : public Type<EnumType>
{
public:
    EnumType(std::string name, EnumConsts consts);
    EnumType(const EnumType &other);

    bool operator==(const EnumType &other) const override;
    bool operator<(const BaseType &other) const override;

    std::string name_;
    EnumConsts consts_;
};

/**
 * Function types
 * e.g. `void (*)(int)`
 */
class FnType final : public Type<FnType>
{
public:
    FnType(Ptr<ParamType> params, Ptr<BaseType> retType);
    FnType(const FnType &other);

    bool operator==(const FnType &other) const override;
    bool operator<(const BaseType &other) const override;

    Ptr<ParamType> params_;
    Ptr<BaseType> retType_;
};

using Params = std::vector<std::pair<std::string, Ptr<BaseType>>>;

/**
 * Parameter types (intermediate type)
 */
class ParamType final : public Type<ParamType>
{
public:
    ParamType(Params types);
    ParamType(const ParamType &other);

    bool operator==(const ParamType &other) const override;
    bool operator<(const BaseType &other) const override;

    size_t size() const;
    const BaseType *at(size_t i) const;

    Params types_;
};

/**
 * Pointer types
 * e.g. `int *`
 */
class PtrType final : public Type<PtrType>
{
public:
    PtrType(Ptr<BaseType> type);
    PtrType(const PtrType &other);

    bool operator==(const PtrType &other) const override;
    bool operator<(const BaseType &other) const override;

    Ptr<BaseType> type_;
};

/**
 * Struct types
 */
class StructType final : public Type<StructType>
{
public:
    enum class Type
    {
        STRUCT,
        UNION
    };

    // Used for forward declarations/self-references
    StructType(Type type, std::string name);
    StructType(Type type, std::string name, Ptr<ParamType> params);
    StructType(const StructType &other);

    bool operator==(const StructType &other) const override;
    bool operator<(const BaseType &other) const override;

    Ptr<BaseType> getMemberType(std::string name) const;
    unsigned int getMemberIndex(std::string name) const;

    Type type_;
    std::string name_;
    Ptr<ParamType> params_;
};

} // namespace AST