#ifndef ast_number_hpp
#define ast_number_hpp

#include "../ast_expression.hpp"
#include <cstring>


/*
 *  Leaf node for numbers (e.g. "10" in "int x = 10;")
*/
class Number : public Expression
{
public:
    Number(double _value) : value(_value) {}

    virtual ~Number()
    {}

    void print(std::ostream &dst, int indent_level) const override
    {
        dst << value;
    }

    double evaluate() const
    {
        return value;
    }

    Types get_type(Context &context) const override
    {
        // Here return the lowest priority type
        return Types::VOID;
    }

    std::string get_id() const override
    {
        return std::to_string(value);
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');

        if (dest_reg[0] == 'f')
        {
            float float_value = (float)value;
            uint32_t ieee754_value;
            std::memcpy(&ieee754_value, &float_value, sizeof(uint32_t));

            std::string label = context.get_unique_label("constant");
            context.add_memory_data(label, ieee754_value);
            std::string lui_reg = context.allocate_register(Types::INT);

            // Accessing main memory to fetch constant value
            dst << indent << "lui " << lui_reg
                << ", %hi(." << label << ")" << std::endl;

            dst << indent << "flw " << dest_reg
                << ", %lo(." << label << ")"
                << "(" << lui_reg << ")" << std::endl;

            context.deallocate_register(lui_reg);
        }
        else
        {
            int val;
            if (context.multiply_pointer)
            {
                val = value * context.pointer_multiplier ;
            }
            else
            {
                val = value;
            }
            dst << indent << "li " << dest_reg << ", " << val << std::endl;
        }
    }

private:
    double value;
};


#endif  /* ast_number_hpp */
