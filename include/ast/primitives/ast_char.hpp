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
        if (_string.size() != 3)
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

    void print(std::ostream &dst, int indent_level) const override
    {
        dst << "'" << string << "'";
    }

    Types get_type() const override
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
        // TODO: make it work for strings larger than 8 chars
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        dst << indent << "li " << dest_reg
            << ", " << (int)string << std::endl;
    }

private:
    char string;
};


#endif  /* ast_char_hpp */
