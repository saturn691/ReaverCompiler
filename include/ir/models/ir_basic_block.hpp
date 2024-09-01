#pragma once

#include <vector>
#include <memory>

#include <ir/models/ir_statement.hpp>
#include <ir/models/ir_terminator.hpp>
#include <ir/models/ir_has_print.hpp>

namespace ir
{
    class BasicBlock : public HasPrint
    {
    public:
        BasicBlock();
        ~BasicBlock() = default;

        BasicBlock(BasicBlock &&other) noexcept;
        BasicBlock &operator=(BasicBlock &&other) noexcept;

        BasicBlock(const BasicBlock &other) = delete;
        BasicBlock &operator=(const BasicBlock &other) = delete;

        void print(std::ostream &dst, int indent_level) const override;

        std::vector<std::unique_ptr<const Statement>> statements;

        // Optional terminator
        std::unique_ptr<Terminator> terminator;
    };

    using BasicBlocks = std::vector<std::unique_ptr<BasicBlock>>;
}
