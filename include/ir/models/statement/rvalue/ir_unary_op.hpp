#pragma once

#include <variant>

#include <ir/models/statement/rvalue/ir_rvalue.hpp>
#include <ir/models/statement/rvalue/ir_constant.hpp>
#include <ir/models/ir_declaration.hpp>

namespace ir
{
    class UnaryOp : public Rvalue
    {
    public:
        UnaryOp(
            const std::string &op,
            const std::variant<Constant, Declaration> &lhs);

        void print(std::ostream &dst, int indent_level) const override;

    private:
        std::string op;
        const std::variant<Constant, Declaration> &lhs;
    };
}
