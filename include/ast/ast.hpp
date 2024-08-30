#pragma once

#include <ast/utils/ast_context.hpp>
#include <ast/utils/ast_utils.hpp>

#include <ast/models/ast_declaration.hpp>
#include <ast/models/ast_expression.hpp>
#include <ast/models/ast_node.hpp>
#include <ast/models/ast_node_list.hpp>
#include <ast/models/ast_statement.hpp>
#include <ast/models/ast_translation_unit.hpp>
#include <ast/models/ast_type.hpp>

namespace ast
{
    extern const TranslationUnit *parseAST(std::string filename);
}
