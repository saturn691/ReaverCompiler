#pragma once

#include <ir/models/traits/ir_has_print.hpp>
#include <ir/models/ir_function.hpp>

namespace ir
{
    class IR : public HasPrint
    {
    public:
        IR() = default;

        void print(std::ostream &dst, int indent_level) const override;

    private:
        // Function definitions
        std::vector<Function> functions;
        // Declarations (globals - structs, enums)
        std::vector<Declaration> declarations;
    };
}
