#pragma once

#include <vector>

#include <ir/models/ir_has_print.hpp>
#include <ir/models/ir_basic_block.hpp>
#include <ir/models/ir_declaration.hpp>
#include <ir/models/ir_statement.hpp>
#include <ir/models/ir_function.hpp>

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

        std::vector<Declaration> locals;
        std::vector<Constant> constants;
    };

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

        const std::string name;
        const Declaration ret;
        const std::vector<Declaration> params;
    };

    class Function : public HasPrint
    {
    public:
        Function(
            const FunctionHeader &header,
            const FunctionLocals &locals,
            BasicBlocks &&bbs);

        Function(const Function &other) = default;

        void print(std::ostream &dst, int indent_level) const override;

        const FunctionHeader header;
        const FunctionLocals locals;
        const BasicBlocks bbs;
    };
}
