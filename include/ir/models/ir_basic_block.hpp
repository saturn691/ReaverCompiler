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

        void print(std::ostream &dst, int indent_level) const override;

    private:
        std::vector<Statement> statements;

        // Optional terminator
        std::unique_ptr<const Terminator> terminator;
    };
}
