#pragma once

#include <variant>

#include <ir/models/statement/rvalue/ir_constant.hpp>
#include <ir/models/statement/rvalue/ir_rvalue.hpp>
#include <ir/models/ir_local_declaration.hpp>

namespace ir
{
    class BinaryOp : public Rvalue
    {
    public:
        BinaryOp(
            const std::variant<Constant, LocalDeclaration> &lhs,
            const std::string &op,
            const std::variant<Constant, LocalDeclaration> &rhs);

        void print(std::ostream &dst, int indent_level) const override;

    private:
        const std::variant<Constant, LocalDeclaration> &lhs;
        const std::string op;
        const std::variant<Constant, LocalDeclaration> &rhs;
    };
}
