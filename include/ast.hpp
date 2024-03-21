#ifndef ast_hpp
#define ast_hpp

#include <ast/ast_context.hpp>
#include <ast/ast_empty_node.hpp>
#include <ast/ast_node.hpp>
#include <ast/ast_declarator.hpp>
#include <ast/ast_init_declarator.hpp>
#include <ast/ast_expression.hpp>

#include <ast/function/ast_function_argument_list.hpp>
#include <ast/function/ast_function_call.hpp>
#include <ast/function/ast_function_declarator.hpp>
#include <ast/function/ast_function_definition.hpp>
#include <ast/function/ast_function_parameter.hpp>
#include <ast/function/ast_function_parameter_list.hpp>

#include <ast/operators/assign/ast_assign_op.hpp>
#include <ast/operators/assign/ast_assign.hpp>

// Ops
#include <ast/operators/ast_operator.hpp>

// Relops
#include <ast/operators/ast_equal.hpp>          // Equal, "=="
#include <ast/operators/ast_less_than_equal.hpp> // Less than or equal to, "<="
#include <ast/operators/ast_less_than.hpp>      // Less than, "<"
#include <ast/operators/ast_logical_and.hpp>    // Logical AND, "&&"
#include <ast/operators/ast_logical_or.hpp>     // Logical OR, "||"
#include <ast/operators/ast_unary_expression.hpp>

// Unary ops
#include <ast/operators/ast_increment.hpp>

// Conditionals
#include <ast/conditionals/ast_if_else.hpp>
#include <ast/conditionals/ast_ternary.hpp>

// Loops
#include <ast/loops/ast_for.hpp>
#include <ast/loops/ast_while.hpp>
#include <ast/loops/ast_do_while.hpp>

// Primitives
#include <ast/primitives/ast_number.hpp>
#include <ast/primitives/ast_identifier.hpp>
#include <ast/primitives/ast_string.hpp>
#include <ast/primitives/ast_char.hpp>

// Array
#include <ast/array/ast_array_declarator.hpp>
#include <ast/array/ast_array_access.hpp>
#include <ast/array/ast_array_initializer_list.hpp>

// Pointers
#include <ast/pointer/ast_pointer.hpp>
#include <ast/pointer/ast_pointer_declarator.hpp>

#include <ast/scopes/ast_scope_manager.hpp>

#include <ast/statements/ast_break.hpp>
#include <ast/statements/ast_continue.hpp>
#include <ast/statements/ast_case.hpp>
#include <ast/statements/ast_return.hpp>
#include <ast/statements/ast_sizeof.hpp>
#include <ast/statements/ast_switch.hpp>

#include <ast/type/ast_type.hpp>
#include <ast/type/ast_basic_type.hpp>
#include <ast/type/enum/ast_enum_definition.hpp>
#include <ast/type/enum/ast_enum_list.hpp>
#include <ast/type/enum/ast_enum_value.hpp>
#include <ast/type/struct/ast_struct_access.hpp>
#include <ast/type/struct/ast_struct_declaration.hpp>
#include <ast/type/struct/ast_struct_declarator_list.hpp>
#include <ast/type/struct/ast_struct_definition.hpp>
#include <ast/type/struct/ast_struct_type.hpp>
#include <ast/type/struct/ast_struct_instance.hpp>

#include <ast/variable/ast_declaration.hpp>

extern const Node *parseAST(std::string filename);

#endif /* ast_hpp */
