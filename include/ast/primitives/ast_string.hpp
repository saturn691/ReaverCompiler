#ifndef ast_string_hpp
#define ast_string_hpp

#include "../ast_node.hpp"
#include "../ast_context.hpp"


/*
 *  Leaf node for string literals (e.g. "10" in "int x = 10;")
*/
class String : public Node
{
public:
    String(std::string _string) :
        string(_string)
    {}

    virtual ~String()
    {}

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        dst << string;
    }

    virtual Types get_type(Context &context) const override
    {
        // This is a string literal, it is an array of chars
        return Types::INT;
    }

    virtual double evaluate(Context &context) const override
    {
        throw std::runtime_error("Cannot evaluate string literal");
    }

    virtual void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        // TODO: make it work for strings larger than 8 chars
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
