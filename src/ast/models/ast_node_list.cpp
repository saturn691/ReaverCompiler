#include <ast/models/ast_node_list.hpp>

NodeList::NodeList() : nodes() {}

NodeList::NodeList(const Node *node)
    : nodes()
{
    nodes.push_back(std::unique_ptr<const Node>(node));
}

void NodeList::push_back(const Node *node)
{
    nodes.push_back(std::unique_ptr<const Node>(node));
}

void NodeList::print(std::ostream &dst, int indent_level) const
{
    for (auto const &node : nodes)
    {
        if (node != nullptr)
        {
            node->print(dst, indent_level);
        }
    }
}

void NodeList::lower(Context &context) const
{
    for (auto const &node : nodes)
    {
        if (node != nullptr)
        {
            node->lower(context);
        }
    }
}

void NodeList::print_delim(
    std::ostream &dst,
    int indent_level,
    const std::string &delim) const
{
    std::string indent = Utils::get_indent(indent_level);
    dst << indent;
    for (size_t i = 0; i < nodes.size(); i++)
    {
        nodes[i]->print(dst, indent_level);
        if (i != nodes.size() - 1)
        {
            dst << delim;
        }
    }
}
