#ifndef ast_struct_instance_hpp
#define ast_struct_instance_hpp

#include "../../ast_node.hpp"


/*
 *  Defines an instance of a struct
 *  (e.g. "struct x y;")
*/
class StructInstance : public Node
{
public:
    StructInstance(std::string _identifier) : identifier(_identifier) {}

    virtual void print(std::ostream &dst, int indent_level) const override
    {
        std::string indent((AST_PRINT_INDENT_SPACES * indent_level), ' ');
        dst << indent << "struct " << identifier;
        // Intentionally no std::endl
    }

private:
    std::string identifier;
};


#endif  /* ast_struct_instance_hpp */
