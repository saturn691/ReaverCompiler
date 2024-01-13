#ifndef ast_struct_type_hpp
#define ast_struct_type_hpp

#include "../ast_type.hpp"
#include <vector>


/*
 *  Defines an instance of a struct
 *  (e.g. "struct x y;")
*/
class StructType : public Type
{
public:
    StructType(
        std::string _identifier,
        std::vector<std::pair<std::string, TypePtr>> _members
    ) :
        identifier(_identifier),
        members(_members)
    {}

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES * indent_level), ' ');
        dst << indent << "struct " << identifier;
        // Intentionally no std::endl
    }

    virtual Types get_type() const override
    {
        return Types::STRUCT;
    }

    virtual void allocate_stack(
        Context &context,
        std::string id
    ) const override {
        // Allocate space for each member
        for (auto member : members)
        {
            std::string new_id = id + "." + member.first;
            Types type = member.second->get_type();
            if (type == Types::STRUCT)
            {
                // Recurse
                member.second->allocate_stack(context, new_id);
            }
            else
            {
                // Allocate space for the member
                context.allocate_stack(type, new_id);
            }
        }
    }

private:
    std::string identifier;
    std::vector<std::pair<std::string, TypePtr>> members;
};


#endif  /* ast_struct_type_hpp */
