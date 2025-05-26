#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <vector>

namespace AST
{
template <typename T>
using Ptr = std::unique_ptr<const T>;

// Forward declarations
class Decl;
class ParamType;
class StructDeclList;
enum class CVRQualifier;
enum class FunctionSpecifier;
enum class Linkage;
enum class StorageDuration;

/**
 * Base class for types.
 */
class BaseType
{
public:
    BaseType();
    BaseType(const BaseType &other);
    virtual ~BaseType() = default;
    virtual bool operator==(const BaseType &other) const = 0;
    virtual bool operator!=(const BaseType &other) const = 0;

    // Compatibility operator. Says nothing about truncation.
    virtual bool operator<(const BaseType &other) const = 0;
    virtual bool operator<=(const BaseType &other) const = 0;
    virtual Ptr<BaseType> clone() const = 0;
    virtual bool isComplete() const noexcept = 0;
    size_t getID() const noexcept;

    size_t id_;
    static size_t idProvider_;

    // Qualifiers (mutable because we use Ptr<> everywhere and cba)
    mutable std::optional<CVRQualifier> cvrQualifier_ = std::nullopt;
    mutable std::optional<FunctionSpecifier> functionSpecifier_ = std::nullopt;
    mutable std::optional<Linkage> linkage_ = std::nullopt;
    mutable std::optional<StorageDuration> storageDuration_ = std::nullopt;
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
        return std::make_unique<Derived>(static_cast<const Derived &>(*this));
    }

    bool operator==(const BaseType &other) const override
    {
        if (auto otherType = dynamic_cast<const Derived *>(&other))
        {
            return static_cast<const Derived &>(*this) == *otherType;
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
    CHAR, // We let char be signed (implementation-defined) for simplicity
    UNSIGNED_CHAR,
    SHORT,
    UNSIGNED_SHORT,
    INT,
    UNSIGNED_INT,
    LONG,
    UNSIGNED_LONG,
    LONG_LONG,
    UNSIGNED_LONG_LONG,
    FLOAT,
    DOUBLE,
    LONG_DOUBLE,
    FLOAT_COMPLEX,
    DOUBLE_COMPLEX,
    LONG_DOUBLE_COMPLEX,
    FLOAT_IMAGINARY,
    DOUBLE_IMAGINARY,
    LONG_DOUBLE_IMAGINARY
};

/**
 * Every object has a storage duration, which limits the object lifetime.
 */
enum class StorageDuration
{
    AUTO,     // Block scope
    STATIC,   // Initialised once, lasts entire execution of the program
    ALLOCATED // Dynamically allocated (ignore)
};

/**
 * Linkage refers to the ability of an identifier (variable or function) to be
 * referred to in other scopes. If a variable or function with the same
 * identifier is declared in several scopes, but cannot be referred to from all
 * of them, then several instances of the variable are generated.
 */
enum class Linkage
{
    NONE,     // Block scope
    INTERNAL, // File scope
    EXTERNAL  // Global scope
};

/**
 * Not mutually exclusive
 */
enum class CVRQualifier
{
    CONST,    // Cannot be modified
    VOLATILE, // Can be modified by something external
    RESTRICT  // No aliasing
};

/**
 * Inline? Compiler hint, we can ignore this.
 */
enum class FunctionSpecifier
{
    INLINE
};

class ArrayType final : public Type<ArrayType>
{
public:
    ArrayType(Ptr<BaseType> type, size_t size);
    ArrayType(const ArrayType &other);

    bool operator==(const ArrayType &other) const override;
    bool operator<(const BaseType &other) const override;

    bool isComplete() const noexcept override;

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
    BasicType(
        Types type,
        std::optional<CVRQualifier> cvrQualifier,
        std::optional<FunctionSpecifier> functionSpecifier,
        std::optional<Linkage> linkage,
        std::optional<StorageDuration> storageDuration);
    BasicType(const BasicType &other);

    bool operator==(const BasicType &other) const override;
    bool operator<(const BaseType &other) const override;

    bool isComplete() const noexcept override;
    bool isSigned() const noexcept;

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

    bool isComplete() const noexcept override;

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

    bool isComplete() const noexcept override;
    std::string getParamName(size_t i) const noexcept;
    const BaseType *getParamType(size_t i) const noexcept;

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

    bool isComplete() const noexcept override;

    size_t size() const noexcept;
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

    bool isComplete() const noexcept override;

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
    StructType(Type type, std::string name, Ptr<ParamType> params, size_t id);
    StructType(const StructType &other);

    bool operator==(const StructType &other) const override;
    bool operator<(const BaseType &other) const override;

    bool isComplete() const noexcept override;

    std::string getName() const noexcept;
    Ptr<BaseType> getMemberType(std::string name) const;
    unsigned int getMemberIndex(std::string name) const;
    Ptr<StructType> withParams(const ParamType *params) const;

    Type type_;
    std::string name_;
    Ptr<ParamType> params_; // Optional
};

} // namespace AST