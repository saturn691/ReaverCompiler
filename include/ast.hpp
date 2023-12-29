#ifndef ast_hpp
#define ast_hpp

#include <ast/ast_binary_node.hpp>
#include <ast/ast_context.hpp>
#include <ast/ast_node.hpp>
#include <ast/ast_type.hpp>

#include <ast/function/ast_function_declarator.hpp>
#include <ast/function/ast_function_definition.hpp>
#include <ast/function/ast_function_parameter.hpp>
#include <ast/function/ast_function_parameter_list.hpp>

#include <ast/operators/assign/ast_assign_op.hpp>
#include <ast/operators/assign/ast_assign.hpp>

// Ops
#include <ast/operators/ast_operator.hpp>
#include <ast/operators/ast_add.hpp>
#include <ast/operators/ast_subtract.hpp>
#include <ast/operators/ast_multiply.hpp>
#include <ast/operators/ast_divide.hpp>

// Relops
#include <ast/operators/ast_bitwise_and.hpp>
#include <ast/operators/ast_bitwise_or.hpp> // Inclusive OR, "|"
#include <ast/operators/ast_bitwise_xor.hpp> // Exclusive OR (XOR), "^"
#include <ast/operators/ast_equal.hpp> // Equal, "=="
#include <ast/operators/ast_less_than_equal.hpp> // Less than or equal to, "<="
#include <ast/operators/ast_less_than.hpp> // Less than, "<"
#include <ast/operators/ast_logical_and.hpp> // Logical AND, "&&"
#include <ast/operators/ast_logical_or.hpp> // Logical OR, "||"

#include <ast/primitives/ast_number.hpp>
#include <ast/primitives/ast_identifier.hpp>

#include <ast/statements/ast_return.hpp>

#include <ast/variable/ast_variable_declaration.hpp>

#include <ast/initializer/ast_simple_initializer.hpp>
// #include <ast/initializer/ast_array_initializer.hpp> // Not implemented yet
// #include <ast/initializer/ast_struct_initializer.hpp> // Not implemented yet

extern const Node *parseAST(std::string filename);

#endif /* ast_hpp */
