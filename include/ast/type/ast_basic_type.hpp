#ifndef ast_basic_type_hpp
#define ast_basic_type_hpp

#include "ast_type.hpp"
#include "../ast_types.hpp"

/*
 *  Leaf node for types (e.g. "double" in "double x = 10")
*/
class BasicType : public Type
{
public:
    BasicType(Types t) : type(t) {}

    virtual ~BasicType()
    {}

    void print(std::ostream &dst, int indent_level) const override
    {
        switch (type)
        {
            case (Types::VOID):
                dst << "void";
                break;
            case (Types::CHAR):
                dst << "char";
                break;
            case (Types::UNSIGNED_CHAR):
                dst << "unsigned char";
                break;
            case (Types::SHORT):
                dst << "short";
                break;
            case (Types::UNSIGNED_SHORT):
                dst << "unsigned short";
                break;
            case (Types::INT):
                dst << "int";
                break;
            case (Types::UNSIGNED_INT):
                dst << "unsigned int";
                break;
            case (Types::LONG):
                dst << "long";
                break;
            case (Types::UNSIGNED_LONG):
                dst << "unsigned long";
                break;
            case (Types::FLOAT):
                dst << "float";
                break;
            case (Types::DOUBLE):
                dst << "double";
                break;
            case (Types::LONG_DOUBLE):
                dst << "long double";
                break;
            default:
                throw std::runtime_error("Undefined type in Type::print()");
                break;
        }
    }

    Types get_type() const override
    {
        return type;
    }

    unsigned int get_size(Context &context) const override
    {
        return Context::type_size_map.at(type);
    }

    void allocate_stack(
        Context &context,
        std::string id
    ) const override {
        context.allocate_stack(type, id, context.is_pointer, context.is_array,
                                context.array_dimensions);
    }

    void gen_asm(
        std::ostream &dst,
        std::string &dest_reg,
        Context &context
    ) const override {
        context.current_declaration_type = this;
        if (context.mode_stack.top() == Context::Mode::SIZEOF)
        {
            unsigned int size = get_size(context);
            dst << "li " << dest_reg << ", " << size << std::endl;
        }
    }

private:
    Types type;
};


#endif  /* ast_basic_type_hpp */
