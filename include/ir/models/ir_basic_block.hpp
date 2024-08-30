#pragma once

#include <vector>
#include <memory>

#include <ir/models/statement/ir_statement.hpp>
#include <ir/models/terminator/ir_terminator.hpp>
#include <ir/models/traits/ir_has_print.hpp>

namespace ir
{
    class BasicBlock : public HasPrint
    {
    public:
        BasicBlock();

        BasicBlock(const BasicBlock &other);

        ~BasicBlock() = default;

        void print(std::ostream &dst, int indent_level) const override;

    private:
        std::vector<std::shared_ptr<Statement>> statements;

        // Optional terminator
        std::shared_ptr<Terminator> terminator;
    };
}
