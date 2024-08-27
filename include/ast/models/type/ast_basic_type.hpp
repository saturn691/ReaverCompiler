#pragma once

#include <ast/models/type/ast_type.hpp>
#include <ast/models/type/ast_types.hpp>

#include <ir/models/ir_type.hpp>

namespace ast
{
    class BasicType : public Type
    {
    public:
        BasicType(const Types type);

        unsigned int get_size() const override;

        ir::Type lower(Context &context) const override;

        void print(std::ostream &dst, int indent_level) const override;

    private:
        Types type;
    };
}
