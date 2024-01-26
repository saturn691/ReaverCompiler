#ifndef ast_char_hpp
#define ast_char_hpp

#include "../ast_node.hpp"
#include "../ast_context.hpp"


/*
 *  Leaf node for char literals (e.g. 'a')
*/
class Char : public Node
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

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        dst << "'" << string << "'";
    }

    virtual Types get_type(Context &context) const override
    {
        return Types::CHAR;
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("Cannot evaluate char literal");
    }

    virtual void gen_asm(
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
