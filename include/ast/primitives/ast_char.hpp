#ifndef ast_char_hpp
#define ast_char_hpp

#include "../ast_expression.hpp"
#include "../ast_context.hpp"


/*
 *  Leaf node for char literals (e.g. 'a')
*/
class Char : public Expression
{
public:
    Char(std::string _string)
    {
        if (_string[1] == '\\')
        {
            auto it = escape_sequences.find(_string[2]);
            if (it != escape_sequences.end())
            {
                string = it->second;
            }
            else
            {
                throw std::runtime_error("Invalid escape sequence");
            }
        }
        else if (_string.size() != 3)
        {
            throw std::runtime_error("Char literal must be a single character");
        }
        else
        {
            string = _string[1];
        }
    }

    virtual ~Char()
    {}

    double evaluate() const override
    {
        return (double)string;
    }

    void print(std::ostream &dst, int indent_level) const override
    {
        dst << "'" << string << "'";
    }

    Types get_type(Context &context) const override
    {
        return Types::UNSIGNED_CHAR;
    }

    std::string get_id() const override
    {
        return std::to_string((int)string);
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        dst << AST_INDENT << "li " << dest_reg
            << ", " << (int)string << std::endl;
    }

private:
    char string;
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
        {'?', '\?'}
    };
};


#endif  /* ast_char_hpp */
