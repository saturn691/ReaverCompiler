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

                unsigned int multiplier = 0;
                recurse_gen_asm(dst, dest_reg, context, type, multiplier);
            }
        }
    }

private:
    /**
     *  Helper function to generate assembly code for array initializer lists
     *  like {{1, 2}, {3, 4}}.
     *
     *  It exploits the fact that the tree is nicely organised such that we only
     *  have to keep track of how many nodes have been visited.
     *
     *  @param dst Output stream to write to
     *  @param base_ptr_reg The register containing the base pointer
     *  @param context The current context
     *  @param type The type of the current declaration
     *  @param index The current index in the list (start at 0)
    */
    void recurse_gen_asm(
        std::ostream &dst,
        std::string &base_ptr_reg,
        Context &context,
        Types type,
        unsigned int &index
    ) const {
        size_t size = nodes.size();
        unsigned int multiplier = context.type_size_map.at(type);
        std::string store = context.get_store_instruction(type);

        // Start
        for (size_t i = 0; i < size; i++)
        {
            const auto &node = nodes[i];
            ArrayInitializerList* list = dynamic_cast<ArrayInitializerList*>(node);

            // Base case
            if (list == nullptr)
            {
                std::string reg = context.allocate_register(
                    dst, type, {});

                node->gen_asm(dst, reg, context);

                dst << AST_INDENT << store << " " << reg << ", "
                    << index << "(" << base_ptr_reg << ")" << std::endl;

                index += multiplier;

                context.deallocate_register(dst, reg);
            }
            // Recursive case
            else
            {
                list->recurse_gen_asm(dst, base_ptr_reg, context, type, index);
            }
        }

    }
};


#endif /* AST_ARRAY_INITIALIZER_LIST_HPP */
