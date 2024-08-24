#pragma once

#include <memory>

#include <ast/models/ast_node.hpp>
#include <ast/models/ast_node_list.hpp>
#include <ast/models/struct/ast_struct.hpp>

class StructDeclaration : public Type, public Node
{
public:
    // Anonymous struct/union
    StructDeclaration(
        const StructOrUnionType type,
        const NodeList *declarations);

    StructDeclaration(
        const StructOrUnionType type,
        const std::string identifier,
        const NodeList *declarations);

    void print(std::ostream &dst, int indent_level) const override;

    void lower(Context &context) const override;

    unsigned int get_size() const override;

private:
    StructOrUnionType type;
    std::string identifier;
    std::unique_ptr<const NodeList> declarations;
};
