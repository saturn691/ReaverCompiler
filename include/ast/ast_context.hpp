#ifndef ast_context_hpp
#define ast_context_hpp

#include <unordered_map>
#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <stack>
#include <deque>
#include <set>
#include <sstream>

#include "ast_types.hpp"
#include "type/ast_type.hpp"
#include "operators/ast_unary_expression.hpp"

#define AST_STACK_ALIGN             16
#define AST_STACK_ALLOCATE          512
#define AST_PRINT_INDENT_SPACES     4
#define AST_ARG_MAX_SIZE            8
#define AST_INDENT                  std::string(AST_PRINT_INDENT_SPACES, ' ')


enum class Scope
{
    GLOBAL,
    LOCAL
};


/*
 *  Identifiers can be used to refer to variables or functions.
 *  However, we don't really know. A union could have been used, maybe in
 *  2 pass compiler. Right now, this is kinda a mess.
 *
 *  TODO if you're reading this and using this as inspiration, please don't.
 *  The 2D array support is just hacked together, the deadline is tomorrow.
*/
struct FunctionVariable
{
    int stack_location;
    Types type;
    std::vector<Types> parameter_types = {};
    bool is_pointer = false;
    bool is_array = false;
    std::vector<int> array_dimensions = {}; // For multi-dimensional arrays
    Scope scope;
};

typedef std::unordered_map<std::string, FunctionVariable> id_map_t;

struct EnumType
{
    std::string name;
    std::vector<std::pair<std::string, unsigned>> values;
};


/*
 *  Stores the states (e.g. variable lookup tables) and registers under usage.
 *  Manages the stack allocation.
*/
class Context
{
public:
    Context();

    std::string allocate_register(
        std::ostream &dst,
        Types type,
        std::vector<std::string> exclude
    );

    std::string allocate_return_register(Types type);

    std::string allocate_arg_register(Types type, std::string id = "");

    void deallocate_register(std::ostream &dst, std::string register_name);

    void push_registers(std::ostream& dst, std::string exclude = "");

    void pop_registers(std::ostream& dst);

    void deallocate_arg_registers();

    void init_stack(std::ostream& dst);

    void end_stack(std::ostream& dst);

    int allocate_stack(
        Types type,
        std::string id,
        bool is_ptr = false,
        bool is_array = false,
        std::vector<int> array_dimensions = {}
    );

    int allocate_bottom_stack(Types type, std::string id);

    int push_identifier_map();

    int pop_identifier_map();

    int push_stack(int bytes);

    void pop_stack(int bytes);

    void reset_frame_pointer();

    void reset_stack_pointer();

    void reset_registers();

    std::string get_unique_label(std::string prefix = "");

    void add_string_data(std::string label, std::string value);

    void gen_memory_asm(std::ostream& dst);

    void add_function_declaration(std::string id);

    void add_function_declaration_type(Types type);

    FunctionVariable get_function_variable(std::string id) const;

    Types get_type(std::string id) const;

    unsigned int get_size(std::string id) const;

    int get_stack_location(std::string id) const;

    void add_enum_value(std::string id, int val = -1);

    void add_enum(std::string id);

    int get_enum_value(std::string id) const;

    static std::string get_load_instruction(Types type);

    static std::string get_store_instruction(Types type);

    void store(
        std::ostream &dst,
        std::string reg,
        std::string id,
        Types type,
        std::string addr_reg = ""
    );

    void load(
        std::ostream &dst,
        std::string reg,
        std::string id,
        Types type,
        std::string label = ""
    );

    /*
    When declaring a variable or a function, say int x, y, z;, we need to know
    the type of x, y, and z. However, because the compiler uses in-order
    traversal, we traverse [int x, y, z] first then [x] then [y] then [z].
    There is no way for [x] to get the information down from it's parent, so
    we define this to pass down this information.
    */
    // The type of the current variable/function declaration.
    TypePtr current_declaration_type;
    bool is_pointer = false;
    bool is_array = false;
    std::vector<int> array_dimensions = {};
    std::string current_id;

    // Used for creation of structs. Keep the current_declaration_type as the
    // struct type, and the current_sub_declaration_type as the member type.
    TypePtr current_sub_declaration_type;
    std::vector<std::pair<std::string, TypePtr>> struct_members;

    // Contains the map of identifiers to struct types
    std::unordered_map<std::string, TypePtr> struct_map;
    std::unordered_map<std::string, TypePtr> id_to_struct;

    // Contains the enum classes. Vector to support anonymous enums
    std::vector<EnumType> enum_map;
    std::vector<std::pair<std::string, unsigned>> enum_values;

    // Where are we right now in the AST?
    // e.g. declaring int x in a struct is different from declaring int x in a
    // function.
    enum class Mode
    {
        STRUCT,
        SWITCH,

        GLOBAL,
        LOCAL,

        ASSIGN,                     // Used for pointers
        LOCAL_DECLARATION,          // Used for declarations
        GLOBAL_DECLARATION,         // Used for global declarations
        INIT_DECLARATION,           // Used for initialising declarations
        SIZEOF,                     // Used for sizeof()
        OPERATOR,                   // Used for operators
        ADDRESS,                    // Used for the & operator
        FUNCTION_DEFINITION,        // Duh
        RETURN                      // Duh as well
    };

    std::stack<Mode> mode_stack;
    bool has_mode(Mode mode) const;

    // Used for continue statements
    std::stack<std::string> continue_label_stack;

    // Used for breaking out of switch-case statements
    std::stack<std::string> end_label_stack;

    // Used for switch-case statements
    std::string switch_reg;

    // Holds the conditions and statements for switch-case statements
    std::stringstream switch_cases_expr;
    std::stringstream switch_default;

    // Used for global variables
    UnaryOperator unary_operator;

    // Stack of maps
    std::stack<id_map_t> map_stack;

    // Contains the .rodata section to be spit out at the end
    std::stringstream memory_map;

    // Static Constants --------------------------------------------------------

    // Map from type to size in bytes
    static const std::unordered_map<Types, unsigned int> type_size_map;

    static const std::unordered_map<Types, std::string> assembler_directive;

private:

    std::string spill_register(
        std::ostream &dst,
        Types type,
        std::vector<std::string> exclude
    );

    void unspill_register(std::ostream &dst, std::string spilled_register);

    // For register spilling
    std::deque<std::string> used_registers;
    std::set<std::string> spilled_registers;

    // Contains the map of labels to string values
    std::unordered_map<std::string, std::string> string_map;

    // The next value to give an enum class
    unsigned int enum_next_value = 0;

    // Integer registers
    std::array<int, 32> registers;

    // Floating point registers
    std::array<int, 32> registers_f;

    // Points to the bottom of the data in the frame
    int frame_pointer_offset = 0;

    // Function call w.r.t sp, function definition w.r.t s0
    int stack_pointer_offset = 0;

    std::unordered_map<std::string, int> label_map;

    // Constants --------------------------------------------------------------

    // Starting values for integer registers
    const std::array<int, 32> registers_default = {
                                        // REG      ABI     DESCRIPTION
        1,                              // x0       zero    zero constant
        1,                              // x1       ra      return address
        1,                              // x2       sp      stack pointer
        1,                              // x3       gp      global pointer
        1,                              // x4       tp      thread pointer
        0, 0, 0,                        // x5-x7    t0-t2   temporary regs
        1, 1,                           // x8-x9    s0-s1   callee-saved regs
        0, 0, 0, 0, 0, 0, 0, 0,         // x10-x17  a0-a7   argument regs
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // x18-x27  s2-s11  callee-saved regs
        0, 0, 0, 0,                     // x28-x31  t3-t6   temporary regs
    };

    // Starting values for floating point registers
    const std::array<int, 32> registers_f_default = {
                                        // REG      ABI     DESCRIPTION
        0, 0, 0, 0, 0, 0, 0, 0,         // f0-7     ft0-7   temporaries
        1, 1,                           // f8-9     fs0-1   saved regs
        0, 0,                           // f10-11   fa0-1   arguments/return val
        0, 0, 0, 0, 0, 0,               // f12-17   fa2-7   arguments
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // f18-27   fs2-11  saved regs
        0, 0, 0, 0                      // f28-31   ft8-11  temporaries
    };

    // Register map from name to index
    const std::unordered_map<std::string, int> register_map = {
        {"zero", 0},
        {"ra", 1},
        {"sp", 2},
        {"gp", 3},
        {"tp", 4},
        {"t0", 5},
        {"t1", 6},
        {"t2", 7},
        {"s0", 8},
        {"s1", 9},
        {"a0", 10},
        {"a1", 11},
        {"a2", 12},
        {"a3", 13},
        {"a4", 14},
        {"a5", 15},
        {"a6", 16},
        {"a7", 17},
        {"s2", 18},
        {"s3", 19},
        {"s4", 20},
        {"s5", 21},
        {"s6", 22},
        {"s7", 23},
        {"s8", 24},
        {"s9", 25},
        {"s10", 26},
        {"s11", 27},
        {"t3", 28},
        {"t4", 29},
        {"t5", 30},
        {"t6", 31}
    };

    // Floating point register map from name to index
    const std::unordered_map<std::string, int> register_map_f = {
        {"ft0", 0},
        {"ft1", 1},
        {"ft2", 2},
        {"ft3", 3},
        {"ft4", 4},
        {"ft5", 5},
        {"ft6", 6},
        {"ft7", 7},
        {"fs0", 8},
        {"fs1", 9},
        {"fa0", 10},
        {"fa1", 11},
        {"fa2", 12},
        {"fa3", 13},
        {"fa4", 14},
        {"fa5", 15},
        {"fa6", 16},
        {"fa7", 17},
        {"fs2", 18},
        {"fs3", 19},
        {"fs4", 20},
        {"fs5", 21},
        {"fs6", 22},
        {"fs7", 23},
        {"fs8", 24},
        {"fs9", 25},
        {"fs10", 26},
        {"fs11", 27},
        {"ft8", 28},
        {"ft9", 29},
        {"ft10", 30},
        {"ft11", 31}
    };
};


#endif /* ast_context_hpp */
