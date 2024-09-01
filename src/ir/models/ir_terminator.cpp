#include <ir/models/ir_terminator.hpp>

namespace ir
{
    /*************************************************************************
     * Return implementation
     ************************************************************************/

    void Return::print(std::ostream &dst, int indent_level) const
    {
        std::string indent = get_indent(indent_level);
        dst << indent << "return" << std::endl;
    }
}
