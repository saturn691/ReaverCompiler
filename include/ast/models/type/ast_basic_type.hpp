#pragma once

#include <ast/models/type/ast_type.hpp>
#include <ast/models/type/ast_types.hpp>

namespace ast
{
    class BasicType : public Type
    {
    public:
        BasicType(const Types type);

        unsigned int get_size() const override;

        void print(std::ostream &dst, int indent_level) const override;

    private:
        Types type;
    };
}
