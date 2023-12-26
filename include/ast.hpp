#ifndef ast_hpp
#define ast_hpp

#include <ast/ast_node.hpp>
#include <ast/ast_context.hpp>
#include <ast/ast_type.hpp>

#include <ast/function/ast_function_definition.hpp>

#include <ast/primitives/ast_number.hpp>
#include <ast/primitives/ast_identifier.hpp>

#include <ast/statements/ast_return.hpp>

extern const Node *parseAST(std::string filename);

#endif /* ast_hpp */
