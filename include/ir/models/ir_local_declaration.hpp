#pragma once

#include <optional>

#include <ir/models/traits/ir_has_print.hpp>
#include <ir/models/ir_type.hpp>

namespace ir
{
    /**
     * A local variable, parameter, or return value declaration.
     */
    class Declaration : public HasPrint
    {
    public:
        Declaration() = default;

        Declaration(const std::optional<std::string> &name, const Type &type);

        void print(std::ostream &dst, int indent_level) const override;

    private:
        std::optional<std::string> name;
        Type type;
    };
}
