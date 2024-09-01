#include <ast/utils/ast_utils.hpp>
#include <ast/models/ast_type.hpp>

namespace ast
{
    std::string Utils::get_indent(int indent_level)
    {
        return std::string(indent_level * spaces, ' ');
    }

    ir::Declaration Utils::get_temp_decl(
        Types type,
        ir::FunctionLocals &locals)
    {
        static int temp_count = 0;

        ir::Declaration decl = ir::Declaration(
            "temp_" + std::to_string(temp_count++),
            BasicType::to_ir_type(type));

        locals.locals.push_back(decl);
        return decl;
    }
}
