#pragma once

#include <unordered_map>

#include <ast/models/ast_node.hpp>

namespace ast
{
    class Char : public Node
    {
    public:
        Char(const std::string _string);

        virtual void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context) const;

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
}
