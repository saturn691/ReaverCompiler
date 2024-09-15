#pragma once

#include <memory>
#include <string>

#include <ir/models/ir_declaration.hpp>

#include <ty/ty.hpp>

namespace ast
{
    // Forward declarations - not ideal but whatever
    enum class Types;
    class Type;
    class Declarator;
    using Types_t = ty::Types;

    /**
     * Contextual information to be passed down when recursively searching through
     * the nodes.
     */
    class Context
    {
    public:
        Context() = default;
        ~Context() = default;

        Types_t get_type(const std::string &id) const;

        ir::Declaration &add_declaration(
            const std::unique_ptr<const Type> &ty,
            const std::unique_ptr<const Declarator> &decl);

        ir::Declaration &get_temp_decl(ty::Types type);

        // This is the same signature as FunctionLocals.declarations
        // So we insert these at the end
        std::vector<ir::Declaration> decls;

        int bb;
    };
}
