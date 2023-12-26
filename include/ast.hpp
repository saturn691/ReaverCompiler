#ifndef ast_hpp
#define ast_hpp

#include <ast/ast_binary_node.hpp>
#include <ast/ast_context.hpp>
#include <ast/ast_node.hpp>
#include <ast/ast_type.hpp>

#include <ast/function/ast_function_definition.hpp>

#include <ast/operators/assign/ast_assign_op.hpp>
#include <ast/operators/assign/ast_assign.hpp>

#include <ast/operators/ast_operator.hpp>
#include <ast/operators/ast_add.hpp>

#include <ast/primitives/ast_number.hpp>
#include <ast/primitives/ast_identifier.hpp>

#include <ast/statements/ast_return.hpp>

#include <ast/variable/ast_variable_declaration.hpp>

extern const Node *parseAST(std::string filename);

#endif /* ast_hpp */
