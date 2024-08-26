#pragma once

#include <vector>

#include <ir/models/terminator/ir_terminator.hpp>
#include <ir/models/ir_local_declaration.hpp>
#include <ir/models/ir_basic_block.hpp>

namespace ir
{
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
}
