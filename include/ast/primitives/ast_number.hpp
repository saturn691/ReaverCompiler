#ifndef ast_number_hpp
#define ast_number_hpp

#include "../ast_expression.hpp"
#include <cstring>
#include <iomanip>


/*
 *  Leaf node for numbers (e.g. "10" in "int x = 10;")
*/
class Number : public Expression
{
public:
    Number(std::string _value) : value(_value) {}

    virtual ~Number()
    {}

    void print(std::ostream &dst, int indent_level) const override
    {
        dst << value;
    }

    double evaluate() const override
    {
        return std::stod(value);
    }

    /**
     *  Refer to sections 6.4.4.1 and 6.4.4.2 of the ISO C90 standard
    */
    Types get_type(Context &context) const override
    {
        char suffix = value.back();
        if (suffix == 'u' || suffix == 'U')
        {
            return Types::UNSIGNED_INT;
        }
        else if (suffix == 'l' || suffix == 'L')
        {
            return Types::LONG;
        }
        else if (suffix == 'f' || suffix == 'F')
        {
            return Types::FLOAT;
        }

        /*
            We need more information. If there is a decimal point, it is a
            double. Why?

            Section 6.4.4.2, paragraph 2:
            "A floating constant has a significant part, which may be followed
            by an exponent part and a suffix that specifies its type."

            Section 6.4.4.2, paragraph 4:
            "An unsuffixed floating constant has type double. If suffixed by the
            letter f or F, it has type float. If suffixed by the letter l or L,
            it has type long double."
        */
        if (value.find('.') != std::string::npos ||
            value.find('e') != std::string::npos ||
            value.find('E') != std::string::npos
        ) {
            return Types::DOUBLE;
        }


        // Make an educated guess
        return Types::INT;
    }

    std::string get_id() const override
    {
        return value;
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        // Could come from a unary expression (e.g. int x = -1;)
        // Therefore use the method has_mode()
        if (context.has_mode(Context::Mode::GLOBAL_DECLARATION))
        {
            Types type = context.current_declaration_type->get_type();
            if (type == Types::DOUBLE)
            {
                std::string value = get_value(type);
                std::string lower_value = value.substr(0, value.find(" "));
                std::string upper_value = value.substr(value.find(" ") + 1);

                dst << AST_INDENT << ".word " << lower_value << std::endl;
                dst << AST_INDENT << ".word " << upper_value << std::endl;
            }
            else
            {
                dst << AST_INDENT << context.assembler_directive.at(type)
                    << " " << get_value(type) << std::endl;
            }
        }
        else if (dest_reg[0] == 'f')
        {
            // Must be a float or a double
            Types type = (get_type(context) == Types::FLOAT)
                        ? Types::FLOAT : Types::DOUBLE;
            std::string label = context.get_unique_label(".constant");
            std::string ieee754_value = get_value(type);

            if (type == Types::FLOAT)
            {
                context.memory_map << AST_INDENT << ".align 2" << std::endl;
                context.memory_map << label << ": " << std::endl;
                context.memory_map << AST_INDENT << ".word"
                    << " " << ieee754_value << std::endl;
            }
            else
            {
                context.memory_map << AST_INDENT << ".align 3" << std::endl;
                context.memory_map << label << ": " << std::endl;
                context.memory_map << AST_INDENT << ".word"
                    << " " << ieee754_value.substr(0, ieee754_value.find(" "))
                    << std::endl;
                context.memory_map << AST_INDENT << ".word"
                    << " " << ieee754_value.substr(ieee754_value.find(" ") + 1)
                    << std::endl;
            }

            context.load(dst, dest_reg, "", type, label);
        }
        else
        {
            long val;
            val = std::stol(value);

            dst << AST_INDENT << "li " << dest_reg << ", "
                << val << std::endl;
        }
    }

private:
    std::string value;

    /**
     *  Converts the value into the correct type, according to the ANSI C90
     *  standard.
     *
     *  Refer to sections 6.4.4.1 (integer constants) and 6.4.4.2 (floating
     *  constants)
     *
     *  @param type The type to convert the value to
     *
     *  @return The value as a string. If the type is a double it will return
     *  two values separated by a space.
    */
    std::string get_value(Types type) const
    {
        float float_value;
        double double_value;
        uint32_t ieee754_value;
        int32_t lower_part, upper_part;
        uint64_t ieee754_double_value;

        long long val = std::stoll(value);
        std::string val_str = "";

        switch (type)
        {
            case Types::UNSIGNED_CHAR:
            case Types::CHAR:
                val_str = std::to_string((int)val);
                break;

            case Types::UNSIGNED_SHORT:
            case Types::SHORT:
                val_str = std::to_string((int)val);
                break;

            case Types::UNSIGNED_INT:
            case Types::INT:
                val_str = std::to_string((int)val);
                break;

            case Types::UNSIGNED_LONG:
                break;

            case Types::LONG:
                val_str = std::to_string(val);
                break;

            case Types::FLOAT:
                float_value = std::stof(value);
                std::memcpy(&ieee754_value, &float_value, sizeof(uint32_t));
                val_str = std::to_string(ieee754_value);
                break;

            case Types::DOUBLE:
                double_value = std::stod(value);
                std::memcpy(&ieee754_double_value, &double_value, sizeof(uint64_t));
                lower_part = static_cast<int32_t>(ieee754_double_value & 0xFFFFFFFF);
                upper_part = static_cast<int32_t>(ieee754_double_value >> 32);
                val_str = std::to_string(lower_part) + " " + std::to_string(upper_part);
                break;

            default:
                throw std::runtime_error("Invalid type for number");
        }

        return val_str;
    }
};


#endif  /* ast_number_hpp */
