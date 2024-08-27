#pragma once

#include <iostream>

namespace ir
{
    class HasPrint
    {
    public:
        virtual ~HasPrint() = default;

        // Prints the node to the output stream
        virtual void print(std::ostream &dst, int indent_level) const = 0;

        inline std::string get_indent(int indent_level) const
        {
            return std::string(indent_level * 4, ' ');
        }
    };
}
