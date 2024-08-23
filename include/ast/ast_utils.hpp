#pragma once

#include <string>

class Utils
{
public:
    static std::string get_indent(int indent_level);

private:
    static const int spaces = 4;
};
