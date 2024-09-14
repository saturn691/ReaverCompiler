#include <ast/utils/ast_context.hpp>
#include <ast/models/ast_type.hpp>

namespace ast
{
    Types_t Context::get_type(const std::string &id) const
    {
        return ty::Types::INT;
    }
}
