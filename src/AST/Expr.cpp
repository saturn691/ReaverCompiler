#include "AST/Expr.hpp"
#include "AST/Type.hpp"

namespace AST
{
Constant::Constant(std::string value) : value_(std::move(value))
{
}

char Constant::getChar() const
{
    if (value_.size() == 3)
    {
        return value_[1];
    }
    if (value_[1] == '\\')
    {
        return escapeChars.at(value_[2]);
    }

    return 0;
}

const std::unordered_map<char, char> Constant::escapeChars = {
    {'a', '\a'},
    {'b', '\b'},
    {'f', '\f'},
    {'n', '\n'},
    {'r', '\r'},
    {'t', '\t'},
    {'v', '\v'},
    {'\\', '\\'},
    {'\'', '\''},
    {'\"', '\"'},
    {'?', '\?'},
    {'0', '\0'}};

} // namespace AST