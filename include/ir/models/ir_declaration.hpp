#pragma once

#include <ir/models/ir_has_print.hpp>
#include <ir/models/ir_type.hpp>

#include <llvm/IR/Value.h>

namespace ir
{
    // Forward declaration
    class Visitor;

    /**
     * A local variable, parameter, or return value declaration.
     */
    class Declaration : public HasPrint
    {
    public:
        Declaration(const std::string &name, const Type &type);

        void print(std::ostream &dst, int indent_level) const override;

        std::string accept(Visitor &visitor) const;

        std::string name;
        Type type;
    };
}
