#pragma once

#include <memory>
#include <vector>
#include <map>

#include <ir/models/ir_declaration.hpp>
#include <ir/models/ir_has_print.hpp>
#include <ir/models/ir_statement.hpp>

namespace ir
{
class BasicBlock;
class Visitor;

/**
 * Base class for all terminators in the IR
 */
class Terminator : public HasPrint
{
public:
    virtual ~Terminator() = default;

    virtual void print(std::ostream &dst, int indent_level) const override = 0;

    virtual void accept(Visitor &visitor) const = 0;
};

/**
 * Represents a call to a function
 */
class Call : public Terminator
{
public:
    Call(const std::string &func,
        const std::vector<std::string> &args,
        const Declaration &ret,
        const BasicBlock &bb);

    void print(std::ostream &dst, int indent_level) const override;

    void accept(Visitor &visitor) const override;

private:
    std::string func;
    std::vector<std::string> args;
    const Declaration &ret;
    const BasicBlock &bb;
};

/**
 * Represents an unconditional branch
 */
class Goto : public Terminator
{
public:
    Goto(int bb);

    void print(std::ostream &dst, int indent_level) const override;

    void accept(Visitor &visitor) const override;

private:
    // Pointer to a BasicBlock inside ir::Function
    int bb;
};

/**
 * Represents a conditional branch
 */
class SwitchInt : public Terminator
{
public:
    SwitchInt(std::unique_ptr<const Rvalue> expr,
        std::map<int, int> vals_to_bb);

    void print(std::ostream &dst, int indent_level) const override;

    void accept(Visitor &visitor) const override;

    // The expression to switch on
    const std::unique_ptr<const Rvalue> expr;
    // Values to a reference to BasicBlock (in vector)
    const std::map<int, int> vals_to_bb;
};

/**
 * Represents a return statement
 */
class Return : public Terminator
{
public:
    Return() = default;

    void print(std::ostream &dst, int indent_level) const override;

    void accept(Visitor &visitor) const override;
};

} // namespace ir
