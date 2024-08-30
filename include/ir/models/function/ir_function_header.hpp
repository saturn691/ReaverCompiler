#pragma once

#include <vector>

#include <ir/models/traits/ir_has_print.hpp>

#include <ir/models/ir_declaration.hpp>

namespace ir
{
    /**
     * Helper class to represent a function header in IR.
     */
    class FunctionHeader : public HasPrint
    {
    public:
        FunctionHeader(
            const std::string name,
            const Declaration ret,
            const std::vector<Declaration> params);

        FunctionHeader(const FunctionHeader &other) = default;

        void print(std::ostream &dst, int indent_level) const override;

    private:
        const std::string name;
        const Declaration ret;
        const std::vector<Declaration> params;
    };
}
