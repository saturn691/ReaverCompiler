#include "ir/models/ir_basic_block.hpp"
#include <ast/models/ast_declaration.hpp>
#include <ast/models/ast_type.hpp>
#include <ast/utils/ast_context.hpp>
#include <memory>

namespace ast
{
Context::Context()
{
    id_to_type.push_back(std::map<std::string, Types_t>());
}

Types_t Context::get_type(std::string id) const
{
    // Search through all scopes
    for (const auto &mp : id_to_type)
    {
        auto it = mp.find(id);
        if (it != mp.end())
        {
            return it->second;
        }
    }

    // Search through function arguments
    auto it = arg_to_type.find(id);
    if (it != arg_to_type.end())
    {
        return it->second;
    }

    throw std::runtime_error("Not found in context: " + id);
}

void Context::set_type(std::string id, Types_t type)
{
    id_to_type.back()[std::move(id)] = std::move(type);
}

void Context::set_arg_type(std::string id, Types_t type)
{
    arg_to_type[std::move(id)] = std::move(type);
}

void Context::clear_args()
{
    arg_to_type.clear();
}

void Context::push_type_scope()
{
    id_to_type.push_back(std::map<std::string, Types_t>());
}

void Context::pop_type_scope()
{
    id_to_type.pop_back();
}

ir::Declaration &Context::add_declaration(
    const std::unique_ptr<const Type> &ty,
    const std::unique_ptr<const Declarator> &decl)
{
    std::string id = decl->get_id();
    ir::Type type = ty->lower();

    decls.emplace_back(ir::Declaration(id, type));
    set_type(id, type.type);

    return decls.back();
}

ir::Declaration &Context::get_temp_decl(Types_t type)
{
    static int temp_count = 0;

    decls.emplace_back(ir::Declaration(
        "temp_" + std::to_string(temp_count++), ir::Type(type)));

    return decls.back();
}

int Context::create_new_bb(ir::BasicBlocks &bbs)
{
    bbs.push_back(std::make_unique<ir::BasicBlock>());
    bb = bbs.size() - 1;
    return bb;
}
} // namespace ast
