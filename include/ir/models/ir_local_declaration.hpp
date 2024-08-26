#pragma once

#include <ir/models/traits/ir_has_print.hpp>
#include <ir/models/ir_type.hpp>

namespace ir
{
    class Declaration : public HasPrint
    {
    public:
        Declaration() = default;

        Declaration(const std::string &name, const Type &type);

        void print(std::ostream &dst, int indent_level) const override;

    private:
        std::string name;
        Type type;
    };
}
