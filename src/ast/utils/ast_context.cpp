#include "ir/models/ir_basic_block.hpp"
#include <ast/models/ast_declaration.hpp>
#include <ast/models/ast_type.hpp>
#include <ast/utils/ast_context.hpp>
#include <memory>

namespace ast
{
    Types_t Context::get_type(const std::string &id) const
    {
        return ty::Types::INT;
    }

    ir::Declaration &Context::add_declaration(
        const std::unique_ptr<const Type> &ty,
        const std::unique_ptr<const Declarator> &decl)
    {
        decls.emplace_back(ir::Declaration(decl->get_id(), ty->lower()));
        return decls.back();
    }

    ir::Declaration &Context::get_temp_decl(ty::Types type)
    {
        static int temp_count = 0;

        decls.emplace_back(ir::Declaration(
            "temp_" + std::to_string(temp_count++),
            ir::Type(type)));

        return decls.back();
    }

    int Context::create_new_bb(ir::BasicBlocks &bbs)
    {
        bbs.push_back(std::make_unique<ir::BasicBlock>());
        bb = bbs.size() - 1;
        return bb;
    }
}
