#pragma once

#include <ir/models/statement/rvalue/ir_rvalue.hpp>
#include <ir/models/statement/ir_statement.hpp>
#include <ir/models/ir_local_declaration.hpp>

namespace ir
{
    class Assign : public Statement
    {
    public:
        Assign(
            const LocalDeclaration &lhs,
            const Rvalue &rhs);

        void print(std::ostream &dst, int indent_level) const override;

    private:
        const LocalDeclaration &lhs;
        const Rvalue &rhs;
    };
}
