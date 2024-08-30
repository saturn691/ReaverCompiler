#pragma once

#include <vector>

#include <ir/models/traits/ir_has_print.hpp>

#include <ir/models/ir_declaration.hpp>
#include <ir/models/statement/rvalue/ir_constant.hpp>

namespace ir
{
    /**
     * Helper class to represent a function locals in IR.
     */
    class FunctionLocals : public HasPrint
    {
    public:
        FunctionLocals() = default;

        FunctionLocals(
            const std::vector<Declaration> locals,
            const std::vector<Constant> constants);

        void print(std::ostream &dst, int indent_level) const override;

    private:
        std::vector<Declaration> locals;
        std::vector<Constant> constants;
    };
}
