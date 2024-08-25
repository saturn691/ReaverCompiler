#pragma once

#include <ir/models/statement/rvalue/ir_rvalue.hpp>

namespace ir
{
    class Constant : public Rvalue
    {
    public:
        Constant(const std::string &value);

        void print(std::ostream &dst, int indent_level) const override;

    private:
        std::string value;
    };
}
