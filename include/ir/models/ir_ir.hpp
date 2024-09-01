#pragma once

#include <ir/models/ir_has_print.hpp>
#include <ir/models/ir_function.hpp>

namespace ir
{
    class IR : public HasPrint
    {
    public:
        IR() = default;

        void print(std::ostream &dst, int indent_level) const override;

        void add_function(std::unique_ptr<Function> function);

        void add_declaration(std::unique_ptr<Declaration> declaration);

        void lower();

        // Function definitions
        std::vector<std::unique_ptr<Function>> functions;
        // Declarations (globals - structs, enums)
        std::vector<std::unique_ptr<Declaration>> declarations;
    };
}
