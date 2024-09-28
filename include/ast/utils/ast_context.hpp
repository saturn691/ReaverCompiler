#pragma once

#include "ir/models/ir_basic_block.hpp"
#include <map>
#include <memory>
#include <string>

#include <ir/models/ir_declaration.hpp>

#include <ty/ty.hpp>

namespace ast
{
// Forward declarations - not ideal but whatever
class Type;
class Declarator;
using Types_t = ty::CompoundType;

/**
 * Contextual information to be passed down when recursively searching through
 * the nodes.
 */
class Context
{
public:
    Context();
    ~Context() = default;

    Types_t get_type(std::string id) const;
    void set_type(std::string id, Types_t type);

    void set_arg_type(std::string id, Types_t type);
    void clear_args();

    void push_type_scope();
    void pop_type_scope();

    ir::Declaration &add_declaration(
        const std::unique_ptr<const Type> &ty,
        const std::unique_ptr<const Declarator> &decl);

    ir::Declaration &get_temp_decl(Types_t type);

    int create_new_bb(ir::BasicBlocks &bbs);

    // This is the same signature as FunctionLocals.declarations
    // So we insert these at the end
    std::vector<ir::Declaration> decls;

    // Points to the current BasicBlock (doesn't have to be the last)
    int bb = -1;

private:
    // Symbol table for type map. Operates as a stack.
    std::vector<std::map<std::string, Types_t>> id_to_type;
    std::map<std::string, Types_t> arg_to_type;
};
} // namespace ast
