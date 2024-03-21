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
        for (size_t i = 0; i < nodes.size(); i++)
        {
            const auto &node = nodes[i];

            if (context.mode_stack.top() == Context::Mode::GLOBAL_DECLARATION)
            {
                node->gen_asm(dst, dest_reg, context);
            }
            else
            {
                // Dest reg contains the base pointer
                Types type = context.current_declaration_type->get_type();
                std::string reg = context.allocate_register(
                    dst, type, {});

                node->gen_asm(dst, reg, context);

                unsigned int index = i * context.type_size_map.at(type);
                std::string store = context.get_store_instruction(type);

                dst << AST_INDENT << store << " " << reg << ", "
                    << index << "(" << dest_reg << ")" << std::endl;

                context.deallocate_register(dst, reg);
            }
        }
    }
};


#endif /* AST_ARRAY_INITIALIZER_LIST_HPP */
