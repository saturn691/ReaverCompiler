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

/**
 * Basic types
 * e.g. `int`, `float`, `char`
 */
class BasicType final : public Node<BasicType>,
                        public Type<BasicType>,
                        public TypeNode
{
public:
    BasicType(Types type) : type_(type)
    {
    }
    BasicType(const BasicType &other) : type_(other.type_)
    {
    }

    bool operator==(const BasicType &other) const override
    {
        return type_ == other.type_;
    }

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
        ParamType(std::vector<Ptr<BaseType>> types) : types_(std::move(types))
        {
        }
        ParamType(const ParamType &other)
        {
            for (const auto &type : other.types_)
            {
                types_.push_back(type->clone());
            }
        }

        bool operator==(const ParamType &other) const
        {
            if (types_.size() != other.types_.size())
            {
                return false;
            }

            for (size_t i = 0; i < types_.size(); ++i)
            {
                if (!(*types_[i] == *other.types_[i]))
                {
                    return false;
                }
            }

            return true;
        }

        std::vector<Ptr<BaseType>> types_;
    };

    FnType(Ptr<ParamType> params, Ptr<BaseType> retType)
        : params_(std::move(params)), retType_(std::move(retType))
    {
    }

    FnType(const FnType &other)
        : params_(other.params_->cloneAsDerived()),
          retType_(other.retType_->clone())
    {
    }

    bool operator==(const FnType &other) const override
    {
        return params_ == other.params_ && retType_ == other.retType_;
    }

    Ptr<ParamType> params_;
    Ptr<BaseType> retType_;
};

using ParamType = FnType::ParamType;

} // namespace AST