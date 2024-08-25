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

    protected:
        inline void print_indent(std::ostream &dst, int indent_level) const
        {
            std::string indent(indent_level * 4, ' ');
            dst << indent;
        }
    };
}
