#ifndef ast_context_hpp
#define ast_context_hpp

#include <unordered_map>
#include <string>
#include <iostream>
#include <stdexcept>

#include "ast_types.hpp"

#define AST_STACK_ALIGN             16
#define AST_STACK_ALLOCATE          64
#define AST_PRINT_INDENT_SPACES     4

struct Variable
{
    int stack_location;
    Types type;
};


/*
 *  Stores the states (e.g. variable lookup tables) and registers under usage.
 *  Manages the stack allocation.
*/
class Context
{
public:
    std::string allocate_register(Types type)
    {
        switch (type)
        {

            case Types::FLOAT:
            case Types::DOUBLE:
                // Search temporary registers (ft0-ft11)
                for (int i = 0; i <= 11; ++i)
                {
                    std::string register_name = "ft" + std::to_string(i);
                    int reg_index = register_map_f.at(register_name);

                    if (registers_f[reg_index] == 0)
                    {
                        registers_f[reg_index] = 1;
                        return register_name;
                    }
                }
                break;

            case Types::LONG_DOUBLE:
                throw std::runtime_error(
                    "Context::allocate_register() - unrecognised type"
                );
                break;

            default:
                // Search temporary registers (t0-t6)
                for (int i = 0; i <= 6; ++i)
                {
                    std::string register_name = "t" + std::to_string(i);
                    int reg_index = register_map.at(register_name);

                    if (registers[reg_index] == 0)
                    {
                        registers[reg_index] = 1;
                        return register_name;
                    }
                }
                break;
        }
    }

    std::string allocate_arg_register(Types type)
    {
        switch (type)
        {
            case Types::FLOAT:
            case Types::DOUBLE:
                // Search argument registers (fa0-fa7)
                for (int i = 0; i <= 7; ++i)
                {
                    std::string register_name = "fa" + std::to_string(i);
                    int reg_index = register_map_f.at(register_name);

                    if (registers_f[reg_index] == 0)
                    {
                        registers_f[reg_index] = 1;
                        return register_name;
                    }
                }
                break;

            case Types::LONG_DOUBLE:
                throw std::runtime_error(
                    "Context::allocate_arg_register() - not implemented"
                );

            default:
                // Search argument registers (a0-a7)
                for (int i = 0; i <= 7; ++i)
                {
                    std::string register_name = "a" + std::to_string(i);
                    int reg_index = register_map.at(register_name);

                    if (registers[reg_index] == 0)
                    {
                        registers[reg_index] = 1;
                        return register_name;
                    }
                }
                break;
        }
    }

    void deallocate_register(std::string register_name)
    {
        auto it = register_map.find(register_name);
        auto it_f = register_map_f.find(register_name);

        if (it != register_map.end())
        {
            registers[it->second] = 0;
        }
        else if (it_f != register_map_f.end())
        {
            registers_f[it_f->second] = 0;
        }
        else
        {
            throw std::runtime_error(
                "Context::deallocate_register() - unrecognised register"
            );
        }
    }

    void deallocate_arg_registers()
    {
        // Search argument registers (fa0-fa7)
        for (int i = 0; i <= 7; ++i)
        {
            std::string register_name = "fa" + std::to_string(i);
            int reg_index = register_map_f.at(register_name);
            registers_f[reg_index] = 0;
        }

        // Search argument registers (a0-a7)
        for (int i = 0; i <= 7; ++i)
        {
            std::string register_name = "a" + std::to_string(i);
            int reg_index = register_map.at(register_name);
            registers[reg_index] = 0;
        }
    }

    void init_stack(std::ostream& dst)
    {
        /*
        Due to the compiler being one pass, we need to assume that functions
        could be called at any time. Therefore it is necessary to save ra onto
        the stack.

        In the future, when the compiler is multi-pass, this can be optimised
        away when a function call is not required.
        */
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        int ra_location = (AST_STACK_ALLOCATE - 4);
        int s0_location = ra_location - 4;

        // Save the frame pointer (s0) into the top of the stack
        // Note that this is with respect to (sp) not (s0)
        dst << indent << "addi sp, sp, -" << AST_STACK_ALLOCATE << std::endl;
        dst << indent << "sw ra, " << ra_location << "(sp)" << std::endl;
        dst << indent << "sw s0, " << s0_location << "(sp)" << std::endl;
        dst << indent << "addi s0, sp, " << AST_STACK_ALLOCATE << std::endl;

        // Move the fp offset down by one alignment, to align the data
        frame_pointer_offset = -AST_STACK_ALIGN;
    }

    void end_stack(std::ostream& dst)
    {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');
        int ra_location = (AST_STACK_ALLOCATE - 4);
        int s0_location = ra_location - 4;

        // Load the frame pointer (s0) and the return address (ra)
        // Note that this is with respect to (sp) not (s0)
        dst << indent << "lw ra, " << ra_location << "(sp)" << std::endl;
        dst << indent << "lw s0, " << s0_location << "(sp)" << std::endl;
        dst << indent << "addi sp, sp, " << AST_STACK_ALLOCATE << std::endl;
    }

    int allocate_stack(Types type, std::string id = "")
    {
        unsigned int bytes = type_size_map.at(type);

        if (frame_pointer_offset - bytes < -AST_STACK_ALLOCATE)
        {
            throw std::runtime_error(
                "Context::allocate_stack() - Stack Overflow"
            );
        }

        frame_pointer_offset -= bytes;

        if (!id.empty())
        {
            variable_map[id] = {frame_pointer_offset, type};
        }

        return frame_pointer_offset;
    }

    std::string get_unique_label(std::string prefix = "")
    {
        unsigned int id = tag_next_id;
        tag_next_id++;

        std::string tag = prefix + std::to_string(id);

        return tag;
    }

    void add_memory_data(std::string label, int value)
    {
        memory_map.insert(std::make_pair(label, value));
    }

    void gen_memory_asm(std::ostream& dst)
    {
        std::string indent(AST_PRINT_INDENT_SPACES, ' ');

        dst << ".section .rodata" << std::endl;

        std::string id;
        int val;

        for (const auto& [id, val] : memory_map)
        {
            // This is what GCC does.
            dst << ".align 2" << std::endl;
            dst << "." << id << ":" << std::endl;

            // Necessary for floating point representations.
            dst << indent << ".word " << val << std::endl;
        }
    }

    void add_function_declaration(std::string id)
    {
        function_declaration_map.insert({id, std::vector<Types>()});
    }

    void add_function_declaration_type(std::string id, Types type)
    {
        function_declaration_map[id].push_back(type);
    }

    Types get_type(std::string id) const
    {
        return variable_map.at(id).type;
    }

    int get_stack_location(std::string id) const
    {
        return variable_map.at(id).stack_location;
    }

private:
    // Contains the map of identifiers to variable properties (defined above)
    std::unordered_map<std::string, Variable> variable_map;

    // Contains the map of labels to word values
    std::unordered_map<std::string, int> memory_map;

    // Stores the types of function declarations
    std::unordered_map<std::string, std::vector<Types>> function_declaration_map;

    // Integer registers
    std::array<int, 32> registers = {   // REG      ABI     DESCRIPTION
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

    // Floating point registers
    std::array<int, 32> registers_f = { // REG      ABI     DESCRIPTION
        0, 0, 0, 0, 0, 0, 0, 0,         // f0-7     ft0-7   temporaries
        1, 1,                           // f8-9     fs0-1   saved regs
        0, 0,                           // f10-11   fa0-1   arguments/return val
        0, 0, 0, 0, 0, 0,               // f12-17   fa2-7   arguments
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // f18-27   fs2-11  saved regs
        0, 0, 0, 0                      // f28-31   ft8-11  temporaries
    };

    // Points to the bottom of the data in the frame
    int frame_pointer_offset = 0;

    unsigned int tag_next_id = 0;

    // Constants --------------------------------------------------------------

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

    // Map from type to size in bytes
    const std::unordered_map<Types, unsigned int> type_size_map = {
        {Types::VOID,               0},
        {Types::UNSIGNED_CHAR,      1},
        {Types::CHAR,               1},
        {Types::UNSIGNED_SHORT,     2},
        {Types::SHORT,              2},
        {Types::UNSIGNED_INT,       4},
        {Types::INT,                4},
        {Types::UNSIGNED_LONG,      8},
        {Types::LONG,               8},
        {Types::FLOAT,              4},
        {Types::DOUBLE,             8},
        {Types::LONG_DOUBLE,        8}
    };
};


#endif /* ast_context_hpp */
