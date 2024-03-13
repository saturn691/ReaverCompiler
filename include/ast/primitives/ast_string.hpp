#ifndef ast_string_hpp
#define ast_string_hpp

#include "../ast_expression.hpp"
#include "../ast_context.hpp"


/*
 *  Leaf node for string literals (e.g. "hello")
*/
class String : public Expression
{
public:
    String(std::string _string) :
        string(_string)
    {}

    virtual ~String()
    {}

    void print(std::ostream &dst, int indent_level) const override
    {
        dst << string;
    }

    Types get_type(Context &context) const override
    {
        // This is a pointer to a char array
        return Types::INT;
    }

    std::string get_id() const override
    {
        return string;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        /*
            In C, strings are arrays of chars. GCC stores the string literal
            in memory and then loads the address of the base.

            e.g.
            lui	    a5,%hi(.LC0)
	        addi	a5,a5,%lo(.LC0)
            lbu	    a5,0(a5)

            MEMORY ADDRESS | MEMORY LABEL | VALUE
            -------------------------------------
            0x1000         | .LC0         | 'H'
            0x1001         | .LC0 + 1     | 'e'
            0x1002         | .LC0 + 2     | 'l'
            0x1003         | .LC0 + 3     | 'l'
            0x1004         | .LC0 + 4     | 'o'
        */
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        std::string label = context.get_unique_label("string");
        context.add_string_data(label, string);

        // Accessing main memory to fetch string
        dst << indent << "lui " << dest_reg
            << ", %hi(." << label << ")" << std::endl;

        dst << indent << "addi " << dest_reg << ", "
            << dest_reg << ", %lo(." << label << ")" << std::endl;
    }

private:
    std::string string;
};


#endif  /* ast_string_hpp */
