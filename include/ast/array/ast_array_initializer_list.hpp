#ifndef AST_ARRAY_INITIALIZER_LIST_HPP
#define AST_ARRAY_INITIALIZER_LIST_HPP

#include "../ast_node.hpp"
#include "../ast_context.hpp"


class ArrayInitializerList : public NodeList
{
    using NodeList::NodeList;

    void print(std::ostream &dst, int indent) const override
    {
        dst << "{";
        for (auto it = nodes.begin(); it != nodes.end(); it++)
        {
            if (it != nodes.begin())
            {
                dst << ", ";
            }
            (*it)->print(dst, indent);
        }
        dst << "}";
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {

        for (const auto &node : nodes)
        {
            // This is fine if it's global, it's more or less the same
            if (context.mode_stack.top() == Context::Mode::GLOBAL_DECLARATION)
            {
                node->gen_asm(dst, dest_reg, context);
            }
            else
            {

            }
        }
    }
};


#endif /* AST_ARRAY_INITIALIZER_LIST_HPP */
