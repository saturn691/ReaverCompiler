#pragma once

#include <string>

#include <ir/models/ir_declaration.hpp>
#include <ir/models/ir_function.hpp>

#include <ty/ty.hpp>

namespace ast
{
    class Utils
    {
    public:
        static std::string get_indent(int indent_level);

        static void print(
            std::ostream &dst,
            int indent_level,
            const std::string &delim);

        static ir::Declaration get_temp_decl(
            ty::Types type,
            ir::FunctionLocals &locals);

    private:
        static const int spaces = 4;
    };
}
