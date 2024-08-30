#pragma once

#include <vector>

#include <ir/models/traits/ir_has_print.hpp>
#include <ir/models/ir_basic_block.hpp>
#include <ir/models/ir_declaration.hpp>
#include <ir/models/statement/rvalue/ir_constant.hpp>
#include <ir/models/function/ir_function_header.hpp>
#include <ir/models/function/ir_function_locals.hpp>

namespace ir
{
    class Function : public HasPrint
    {
    public:
        Function(
            const FunctionHeader header,
            const FunctionLocals locals,
            const std::vector<BasicBlock> bbs);

        Function(const Function &other) = default;

        void print(std::ostream &dst, int indent_level) const override;

    private:
        const FunctionHeader header;
        const FunctionLocals locals;
        const std::vector<BasicBlock> bbs;
    };
}
