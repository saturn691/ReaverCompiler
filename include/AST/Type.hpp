#pragma once

#include "AST/Node.hpp"

namespace AST
{
/**
 * Base class for types.
 */
class BaseType
{
public:
    virtual ~BaseType() = default;
    virtual bool operator==(const BaseType &other) const = 0;
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
 * Base class for types in the AST.
 */
class TypeNode : public virtual BaseNode, public virtual BaseType
{
public:
    virtual ~TypeNode() = default;
};

/**
 * Standard types
 */
enum class Types
{
    VOID,
    BOOL,
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
class BasicType final : public Node<BasicType>,
                        public Type<BasicType>,
                        public TypeNode
{
public:
    BasicType(Types type);
    BasicType(const BasicType &other);

    bool operator==(const BasicType &other) const override;
    bool operator<(const BaseType &other) const override;

    Types type_;
};

/**
 * Function types
 * e.g. `void (*)(int)`
 */
class FnType final : public Type<FnType>
{
public:
    /**
     * Parameter types (intermediate type)
     */
    class ParamType final : public Type<ParamType>
    {
    public:
        ParamType(std::vector<Ptr<BaseType>> types);
        ParamType(const ParamType &other);

        bool operator==(const ParamType &other) const override;
        bool operator<(const BaseType &other) const override;

        size_t size() const;
        const BaseType *at(size_t i) const;

        std::vector<Ptr<BaseType>> types_;
    };

    FnType(Ptr<ParamType> params, Ptr<BaseType> retType);
    FnType(const FnType &other);

    bool operator==(const FnType &other) const override;
    bool operator<(const BaseType &other) const override;

    Ptr<ParamType> params_;
    Ptr<BaseType> retType_;
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

using ParamType = FnType::ParamType;

} // namespace AST