#pragma once

#include <vector>

#include <ir/models/traits/ir_has_print.hpp>
#include <ir/models/ir_basic_block.hpp>
#include <ir/models/ir_local_declaration.hpp>
#include <ir/models/statement/rvalue/ir_constant.hpp>

namespace ir
{
    class Function : public HasPrint
    {
    public:
        Function(
            const std::string name,
            const Declaration ret,
            const std::vector<Declaration> params);

        Function(const Function &other);

        void print(std::ostream &dst, int indent_level) const override;

    private:
        const std::string name;
        std::vector<BasicBlock> bbs;
        std::vector<Declaration> locals;
        std::vector<Declaration> params;
        const Declaration ret;
        std::vector<Constant> constants;
    };
}
