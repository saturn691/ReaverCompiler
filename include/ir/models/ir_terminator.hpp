#pragma once

#include <vector>

#include <ir/models/ir_has_print.hpp>
#include <ir/models/ir_declaration.hpp>

namespace ir
{
    class BasicBlock;

    /**
     * Base class for all terminators in the IR
     */
    class Terminator : public HasPrint
    {
    public:
        virtual ~Terminator() = default;

        virtual void print(std::ostream &dst, int indent_level) const override = 0;
    };

    /**
     * Represents a call to a function
     */
    class Call : public Terminator
    {
    public:
        Call(
            const std::string &func,
            const std::vector<std::string> &args,
            const Declaration &ret,
            const BasicBlock &bb);

        void print(std::ostream &dst, int indent_level) const override;

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
        Goto(const BasicBlock &bb);

        void print(std::ostream &dst, int indent_level) const override;

    private:
        const BasicBlock &bb;
    };

    /**
     * Represents a conditional branch
     */
    class SwitchInt : public Terminator
    {
    public:
        SwitchInt(const int value, const std::vector<BasicBlock &> &bbs);

        void print(std::ostream &dst, int indent_level) const override;

    private:
        // The value to switch on
        const int value;
        // The targets for each case
        const std::vector<BasicBlock &> &bbs;
    };

    /**
     * Represents a return statement
     */
    class Return : public Terminator
    {
    public:
        Return() = default;

        void print(std::ostream &dst, int indent_level) const override;
    };

}
