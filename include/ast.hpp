#pragma once

#include <ast/ast_context.hpp>
#include <ast/ast_node.hpp>

#include <ast/function/ast_function_definition.hpp>

extern const Node *parseAST(std::string filename);
