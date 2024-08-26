#pragma once

#include <ast/utils/ast_context.hpp>
#include <ast/utils/ast_utils.hpp>
#include <ast/models/ast_models.hpp>

namespace ast
{
    extern const TranslationUnit *parseAST(std::string filename);
}
