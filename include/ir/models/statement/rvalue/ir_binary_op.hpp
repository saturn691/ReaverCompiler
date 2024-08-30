#pragma once

#include <variant>

#include <ir/models/statement/rvalue/ir_constant.hpp>
#include <ir/models/statement/rvalue/ir_rvalue.hpp>
#include <ir/models/ir_declaration.hpp>

namespace ir
{
    class BinaryOp : public Rvalue
    {
    public:
        BinaryOp(
            const std::variant<Constant, Declaration> &lhs,
            const std::string &op,
            const std::variant<Constant, Declaration> &rhs);

        void print(std::ostream &dst, int indent_level) const override;

    private:
        const std::variant<Constant, Declaration> &lhs;
        const std::string op;
        const std::variant<Constant, Declaration> &rhs;
    };
}
