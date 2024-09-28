#pragma once

#include <memory>

#include <ir/models/ir_declaration.hpp>
#include <ir/models/ir_has_print.hpp>

#include <llvm/IR/Value.h>

#include <ty/ty.hpp>

namespace ir
{
// Forward declaration
class Visitor;

/**
 * Base class for all statements in the IR
 */
class Statement : public HasPrint
{
public:
    virtual ~Statement() = default;

    virtual void print(std::ostream &dst, int indent_level) const override = 0;

    virtual llvm::Value *accept(Visitor &visitor) const = 0;
};

/**
 * Base class for all rvalues in the IR
 */
class Rvalue : public HasPrint
{
public:
    virtual void print(std::ostream &dst, int indent_level) const override = 0;

    virtual llvm::Value *accept(Visitor &visitor) const = 0;
};

/**
 * All lvalues in the IR
 * TODO support more than just variables
 */
class Lvalue : public Rvalue
{
public:
    Lvalue(const Declaration decl);

    void print(std::ostream &dst, int indent_level) const override;

    llvm::Value *accept(Visitor &visitor) const override;

    const Declaration decl;
};

/**
 * Represents a call to a function
 */
class Call : public Rvalue
{
public:
    Call(
        std::unique_ptr<const Rvalue> func,
        std::vector<std::unique_ptr<const Rvalue>> args);

    void print(std::ostream &dst, int indent_level) const override;

    llvm::Value *accept(Visitor &visitor) const override;

private:
    const std::unique_ptr<const Rvalue> func;
    const std::vector<std::unique_ptr<const Rvalue>> args;
};

/**
 * Represents a constant value
 */
class Constant : public Rvalue
{
public:
    Constant(const std::string &value, ty::Types type);

    void print(std::ostream &dst, int indent_level) const override;

    llvm::Value *accept(Visitor &visitor) const override;

    const std::string value;
    const ty::Types type;
};

enum class BinaryOpType
{
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    BITWISE_AND,
    BITWISE_OR,
    BITWISE_XOR,
    LSL,
    LSR,
    EQ,
    NE,
    LT,
    GT,
    LE,
    GE,
};

/**
 * Represents a binary operation
 */
class BinaryOp : public Rvalue
{
public:
    BinaryOp(
        std::unique_ptr<const Rvalue> lhs,
        const BinaryOpType &op,
        std::unique_ptr<const Rvalue> rhs);

    void print(std::ostream &dst, int indent_level) const override;

    llvm::Value *accept(Visitor &visitor) const override;

    const std::unique_ptr<const Rvalue> lhs;
    const BinaryOpType op;
    const std::unique_ptr<const Rvalue> rhs;
};

/**
 * Represents an assignment statement
 * e.g. `a = b`
 */
class Assign : public Statement
{
public:
    Assign(Lvalue lhs, std::unique_ptr<const Rvalue> rhs);

    void print(std::ostream &dst, int indent_level) const override;

    llvm::Value *accept(Visitor &visitor) const override;

    const Lvalue lhs;
    const std::unique_ptr<const Rvalue> rhs;
};

/**
 * Represents a use of a variable
 */
class Use : public Rvalue
{
public:
    Use(Declaration decl);

    void print(std::ostream &dst, int indent_level) const override;

    llvm::Value *accept(Visitor &visitor) const override;

    const Declaration decl;
};

/**
 * Represents a cast operation
 * e.g. `(int) a`
 */
class Cast : public Rvalue
{
public:
    Cast(Type type, Use val);

    void print(std::ostream &dst, int indent_level) const override;

    llvm::Value *accept(Visitor &visitor) const override;

    const Type type;
    const Use val;
};
} // namespace ir
