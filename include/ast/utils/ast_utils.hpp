#pragma once

#include <string>

namespace ast
{
    class Utils
    {
    public:
        static std::string get_indent(int indent_level);

        static void print(
            std::ostream &dst,
            int indent_level,
            const std::string &delim);

    private:
        static const int spaces = 4;
    };
}
