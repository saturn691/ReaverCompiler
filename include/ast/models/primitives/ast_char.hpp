#pragma once

#include <unordered_map>

#include <ast/models/ast_node.hpp>

class Char : public Node
{
public:
    Char(const std::string _string);

    virtual void print(std::ostream &dst, int indent_level) const override;

    virtual void lower(Context &context) const override;

private:
    char value;
    const std::unordered_map<char, char> escape_sequences = {
        {'a', '\a'},
        {'b', '\b'},
        {'f', '\f'},
        {'n', '\n'},
        {'r', '\r'},
        {'t', '\t'},
        {'v', '\v'},
        {'0', '\0'},
        {'\\', '\\'},
        {'\'', '\''},
        {'\"', '\"'},
        {'?', '\?'}};
};
