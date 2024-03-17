#include <ast/ast_context.hpp>

const std::unordered_map<Types, unsigned int> Context::type_size_map = {
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



Context::Context() :
    // Vector must be initialised to empty vector here
    struct_members(
        std::vector<std::pair<std::string, TypePtr>>()
    ),
    // Initialise the identifier map stack
    map_stack(
        std::stack<id_map_t>()
    )
{
    map_stack.push(id_map_t());
    mode_stack.push(Mode::GLOBAL);
}


/**
 *  Finds an empty register and marks it as being used. If there are no empty
 *  registers, it will try to spill a register onto the stack.
 *
 *
 *  @param type The type of the register
 *  @return The name of the register
*/
std::string Context::allocate_register(
    std::ostream &dst,
    Types type,
    std::vector<std::string> exclude
) {
    std::string allocated_register;
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
                    allocated_register = register_name;
                    break;
                }
            }
            break;

        // TODO this
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
                    allocated_register = register_name;
                    break;
                }
            }

            if (allocated_register != "")
            {
                break;
            }

            // Search temporary registers (a1-a7)
            // NOTE: THIS IS A TEMPORARY FIX FOR UNOPTIMISED REGISTER USAGE
            for (int i = 1; i <= 7; ++i)
            {
                std::string register_name = "a" + std::to_string(i);
                int reg_index = register_map.at(register_name);

                if (registers[reg_index] == 0)
                {
                    registers[reg_index] = 1;
                    allocated_register = register_name;
                    break;
                }
            }

            break;
    }

    if (allocated_register == "")
    {
        // Spill into stack location, if possible
        allocated_register = spill_register(dst, type, exclude);
    }

    used_registers.push_back(allocated_register);

    return allocated_register;
}


/**
 *  Spills a register onto the stack. Uses a queue and attempts to find a
 *  suitable register at the front of the queue to spill.
*/
std::string Context::spill_register(
    std::ostream &dst,
    Types type,
    std::vector<std::string> exclude
){
    // Search the queue from the front
    for (std::string register_name : used_registers)
    {
        if (std::find(exclude.begin(), exclude.end(), register_name)
            != exclude.end()
        ) {
            continue;
        }

        // Mark is as being spillt
        spilled_registers.emplace(register_name);

        switch (type)
        {
            // Search for any register_name that begins with a 'f'
            case Types::FLOAT:
            case Types::DOUBLE:
            // TODO support long doubles properly
            case Types::LONG_DOUBLE:
                if (register_name[0] == 'f')
                {
                    // Spill the register onto the stack
                    int stack_loc = allocate_stack(type, "!" + register_name);
                    dst << AST_INDENT << "fsw " << register_name
                        << ", " << stack_loc << "(s0)" << std::endl;

                    // Mark the register as being free
                    registers_f[register_map_f.at(register_name)] = 0;

                    return register_name;
                }

            default:
                if (register_name[0] != 'f')
                {
                    // Spill the register onto the stack
                    int stack_loc = allocate_stack(type, "!" + register_name);
                    dst << AST_INDENT << "sw " << register_name
                        << ", " << stack_loc << "(s0)" << std::endl;

                    // Mark the register as being free
                    registers[register_map.at(register_name)] = 0;

                    return register_name;
                }
        }
    }

    // If we cannot pop any registers, we are out of options
    throw std::runtime_error(
        "Context::spill_register() - out of registers"
    );
}


/**
 *  Unspills a register by restoring the spillt register from the stack and
 *  doing some housekeeping.
 *
 *  @param dst The output stream
 *  @param spilled_register The register to unspill
*/
void Context::unspill_register(
    std::ostream &dst,
    std::string spilled_register
) {
    // Search the stack for the spilled register
    for (const auto& pair : map_stack.top())
    {
        if (pair.first[0] == '!')
        {
            std::string dest_reg = pair.first.substr(1);
            if (dest_reg == spilled_register)
            {
                int stack_loc = pair.second.stack_location;
                Types type = pair.second.type;

                switch (type)
                {
                    case Types::FLOAT:
                        dst << AST_INDENT << "flw " << dest_reg << ", "
                            << stack_loc << "(s0)" << std::endl;
                        break;
                    default:
                        dst << AST_INDENT << "lw " << dest_reg << ", "
                            << stack_loc << "(s0)" << std::endl;
                        break;
                }

                // Mark the register as being used
                registers[register_map.at(dest_reg)] = 1;

                // Mark it as being free
                spilled_registers.erase(
                    std::find(
                        spilled_registers.begin(),
                        spilled_registers.end(),
                        dest_reg
                    )
                );

                map_stack.top().erase("!" + dest_reg);
                pop_stack(type_size_map.at(type));
                return;
            }
        }
    }

    // If we cannot find the spilled register, we are out of options
    throw std::runtime_error(
        "Context::unspill_register() - register not found"
    );
}


/**
 *  Gets the return register for a given type and marks it as being used.
 *
 *  @param type The type of the return value
 *  @return a0 or fa0
*/
std::string Context::allocate_return_register(Types type)
{
    std::string return_register;

    switch (type)
    {
        case Types::FLOAT:
        case Types::DOUBLE:
        case Types::LONG_DOUBLE:
            return_register = "fa0";
            registers_f[register_map_f.at(return_register)] = 1;
            break;

        default:
            return_register = "a0";
            registers[register_map.at(return_register)] = 1;
            break;
    }

    return return_register;
}


/**
 *  Allocate a register for an argument. To be used in function calls and
 *  function definitions.
 *
 *  According to the RVG ABI, the first 8 arguments are passed in registers
 *  a0-a7 and fa0-fa7. This function will allocate a register for the argument
 *  and return the register name.
 *
 *  If no registers are available, it will return a stack location to where to
 *  place or find the argument.
 *
 *  @param type The type of the argument
 *  @param id The identifier of the argument
 *
 *  @return The register name or stack location
*/
std::string Context::allocate_arg_register(Types type, std::string id)
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

        // TODO - take two registers that are aligned.
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

    // If no registers are available, return a stack location
    int stack_loc = stack_pointer_offset;
    stack_pointer_offset += type_size_map.at(type);
    map_stack.top()[id] = {stack_loc, type};
    map_stack.top()[id].is_pointer = is_pointer;

    return std::to_string(stack_loc);
}


void Context::deallocate_register(std::ostream &dst, std::string register_name)
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

    // Remove it from the deque
    used_registers.erase(
        std::find(
            used_registers.begin(),
            used_registers.end(),
            register_name
        )
    );

    // If it has been spillt, unspill
    if (spilled_registers.find(register_name) != spilled_registers.end())
    {
        unspill_register(dst, register_name);
    }

    return;
}


/**
 *  Checks if a register is in use, and if so, pushes it to the stack.
 *  Should call the pop_registers() function after the function call.
*/
void Context::push_registers(std::ostream& dst, std::string exclude)
{
    dst << "# Pushing registers onto stack (if any)" << std::endl;

    // Search temporary registers (ft0-ft7)
    for (int i = 0; i <= 11; ++i)
    {
        std::string register_name = "ft" + std::to_string(i);
        int reg_index = register_map_f.at(register_name);
        if (registers_f[reg_index] == 1 && register_name != exclude)
        {
            registers_f[reg_index] = 0;
            // 32-bit registers
            int stack_loc = allocate_stack(
                Types::DOUBLE,
                "!" + register_name
            );
            dst << AST_INDENT << "fsw " << register_name
                << ", " << stack_loc << "(s0)" << std::endl;
        }
    }

    // Search temporary registers (t0-t6)
    for (int i = 0; i <= 6; ++i)
    {
        std::string register_name = "t" + std::to_string(i);
        int reg_index = register_map.at(register_name);
        if (registers[reg_index] == 1 && register_name != exclude)
        {
            registers[reg_index] = 0;
            // 32-bit registers
            int stack_loc = allocate_stack(
                Types::LONG,
                "!" + register_name
            );
            dst << AST_INDENT << "sw " << register_name
                << ", " << stack_loc << "(s0)" << std::endl;
        }
    }

    // Search argument registers (a1-a7)
    for (int i = 0; i <= 7; ++i)
    {
        std::string register_name = "a" + std::to_string(i);
        int reg_index = register_map.at(register_name);
        if (registers[reg_index] == 1 && register_name != exclude)
        {
            registers[reg_index] = 0;
            // 32-bit registers
            int stack_loc = allocate_stack(
                Types::LONG,
                "!" + register_name
            );
            dst << AST_INDENT << "sw " << register_name
                << ", " << stack_loc << "(s0)" << std::endl;
        }
    }
}


void Context::pop_registers(std::ostream& dst)
{

    std::vector<std::string> to_erase;
    dst << "# Popping registers from stack (if any)" << std::endl;

    for (const auto& pair : map_stack.top())
    {
        if (pair.first[0] == '!')
        {
            std::string dest_reg = pair.first.substr(1);
            int stack_loc = pair.second.stack_location;
            Types type = pair.second.type;

            switch (type)
            {
                case Types::DOUBLE:
                    dst << AST_INDENT << "flw " << dest_reg << ", "
                        << stack_loc << "(s0)" << std::endl;
                    break;
                default:
                    dst << AST_INDENT << "lw " << dest_reg << ", "
                        << stack_loc << "(s0)" << std::endl;
                    break;
            }

            registers[register_map.at(dest_reg)] = 1;

            to_erase.push_back("!" + dest_reg);
            pop_stack(8);
        }
    }

    for (std::string e : to_erase)
    {
        map_stack.top().erase(e);
    }
}


void Context::deallocate_arg_registers()
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

    // Reset the stack pointer
    stack_pointer_offset = 0;
}


void Context::init_stack(std::ostream& dst)
{
    /*
    Due to the compiler being one pass, we need to assume that functions
    could be called at any time. Therefore it is necessary to save ra onto
    the stack.

    In the future, when the compiler is multi-pass, this can be optimised
    away when a function call is not required.
    */

    int ra_location = (AST_STACK_ALLOCATE - 4);
    int s0_location = ra_location - 4;

    // Save the frame pointer (s0) into the top of the stack
    // Note that this is with respect to (sp) not (s0)
    dst << AST_INDENT << "addi sp, sp, -" << AST_STACK_ALLOCATE << std::endl;
    dst << AST_INDENT << "sw ra, " << ra_location << "(sp)" << std::endl;
    dst << AST_INDENT << "sw s0, " << s0_location << "(sp)" << std::endl;
    dst << AST_INDENT << "addi s0, sp, " << AST_STACK_ALLOCATE << std::endl;

    // Move the fp offset down by one alignment, to align the data
    frame_pointer_offset = -AST_STACK_ALIGN;
}


void Context::end_stack(std::ostream& dst)
{

    int ra_location = (AST_STACK_ALLOCATE - 4);
    int s0_location = ra_location - 4;

    // Load the frame pointer (s0) and the return address (ra)
    // Note that this is with respect to (sp) not (s0)
    dst << AST_INDENT << "lw ra, " << ra_location << "(sp)" << std::endl;
    dst << AST_INDENT << "lw s0, " << s0_location << "(sp)" << std::endl;
    dst << AST_INDENT << "addi sp, sp, " << AST_STACK_ALLOCATE << std::endl;
}


/**
 *  Allocate a stack location for a variable according to its type and adds it
 *  to the lookup table.
 *
 *  @param type The type of the variable
 *  @param id The identifier of the variable
*/
int Context::allocate_stack(Types type, std::string id)
{
    unsigned int bytes = (is_pointer) ? 4 : type_size_map.at(type);

    int stack_loc = push_stack(bytes);

    map_stack.top()[id] = {stack_loc, type};
    map_stack.top()[id].is_pointer = is_pointer;

    return stack_loc;
}


int Context::allocate_array_stack(Types type, int size, std::string id)
{
    unsigned int bytes = type_size_map.at(type) * size;
    int stack_loc = push_stack(bytes);

    map_stack.top()[id] = {stack_loc, type};
    map_stack.top()[id].is_pointer = is_pointer;

    return stack_loc;
}


int Context::push_identifier_map()
{
    map_stack.push(map_stack.top());
    return map_stack.size() - 1;
}


int Context::pop_identifier_map()
{
    map_stack.pop();
    return map_stack.size() - 1;
}


int Context::push_stack(int bytes)
{
    if (frame_pointer_offset - bytes < -AST_STACK_ALLOCATE)
    {
        throw std::runtime_error(
            "Context::allocate_stack() - Stack Overflow"
        );
    }

    frame_pointer_offset -= bytes;

    return frame_pointer_offset;
}


void Context::pop_stack(int bytes)
{
    frame_pointer_offset += bytes;
}


std::string Context::get_unique_label(std::string prefix)
{
    return prefix + std::to_string(label_map[prefix]++);
}


void Context::add_memory_data(std::string label, int value)
{
    memory_map.insert(std::make_pair(label, value));
}


void Context::add_string_data(std::string label, std::string value)
{
    string_map.insert(std::make_pair(label, value));
}


void Context::gen_memory_asm(std::ostream& dst)
{


    dst << ".section .rodata" << std::endl;

    std::string id;
    int val;

    for (const auto& [id, val] : memory_map)
    {
        // This is what GCC does.
        dst << ".align 2" << std::endl;
        dst << "." << id << ":" << std::endl;

        // Necessary for floating point representations.
        dst << AST_INDENT << ".word " << val << std::endl;
    }

    for (const auto& [id, val] : string_map)
    {
        // This is what GCC does.
        dst << ".align 2" << std::endl;
        dst << "." << id << ":" << std::endl;

        // Necessary for floating point representations.
        dst << AST_INDENT << ".string " << val << std::endl;
    }
}


void Context::add_function_declaration(std::string id)
{
    FunctionVariable function;
    function.stack_location = -1;
    function.type = current_declaration_type->get_type();
    function.parameter_types.clear();
    function.is_pointer = is_pointer;

    map_stack.top().insert({id, function});
    current_id = id;
}


void Context::add_function_declaration_type(Types type)
{
    map_stack.top().at(current_id).parameter_types.push_back(type);
}


bool Context::get_is_pointer(std::string id) const
{
    return map_stack.top().at(id).is_pointer;
}


void Context::set_is_pointer(std::string id, bool is_pointer)
{
    map_stack.top().at(id).is_pointer = is_pointer;
}


Types Context::get_type(std::string id) const
{
    // First, try to find the id in the identifier_map
    auto identifier_it = map_stack.top().find(id);
    if (identifier_it != map_stack.top().end())
    {
        return identifier_it->second.type;
    }

    // If the id is not in the identifier_map, try to find it in the enum_map
    for (const auto& enum_type : enum_map) {
        for (const auto& [enum_id, enum_val] : enum_type.values) {
            if (enum_id == id) {
                return Types::INT;
            }
        }
    }

    // If the id is not in the identifier_map or the enum_map, throw an exception
    throw std::runtime_error("Identifier not found: " + id);
}


int Context::get_stack_location(std::string id) const
{
    return map_stack.top().at(id).stack_location;
}

void Context::add_enum_value(std::string id, int val)
{
    unsigned int real_val = 0;
    if (val == -1)
    {
        real_val = enum_next_value;
        enum_next_value++;
    }
    else
    {
        real_val = val;
    }

    enum_values.push_back({id, real_val});
}


void Context::add_enum(std::string id)
{
    EnumType enum_obj;
    enum_obj.name = id;
    enum_obj.values = enum_values;

    enum_map.push_back(enum_obj);

    // Resets the enum values for next time
    enum_values.clear();
    enum_next_value = 0;
}


int Context::get_enum_value(std::string id) const
{
    for (const auto& enum_type : enum_map)
    {
        for (const auto& [enum_id, enum_val] : enum_type.values)
        {
            if (enum_id == id)
            {
                return enum_val;
            }
        }
    }

    return -1;
}


/**
 *  Traverses the identifier map and returns the size of the variable or if
 *  it is a struct, the total size of the members.
*/
unsigned int Context::get_size(std::string id) const
{
    unsigned int total_size = 0;
    std::string id_dot = id + ".";

    for (const auto& [key, value] : map_stack.top())
    {
        // Either a variable or a struct member
        // Need to search like this otherwise we get false positives
        if (key == id || key.substr(0, id_dot.size()) == id_dot)
        {
            total_size += type_size_map.at(value.type);
        }
    }

    return total_size;
}


std::string Context::get_load_instruction(Types type)
{
    std::string instruction;

    switch (type)
    {
        case Types::UNSIGNED_CHAR:
            instruction = "lbu";
            break;
        case Types::CHAR:
            instruction = "lb";
            break;
        case Types::UNSIGNED_SHORT:
            instruction = "lhu";
            break;
        case Types::SHORT:
            instruction = "lh";
            break;
        case Types::UNSIGNED_INT:
        case Types::INT:
        case Types::UNSIGNED_LONG:
        case Types::LONG:
            instruction = "lw";
            break;
        case Types::FLOAT:
            instruction = "flw";
            break;
        case Types::DOUBLE:
        case Types::LONG_DOUBLE:
            instruction = "fld";
            break;
        default:
            throw std::runtime_error(
                "Context::get_load_instruction() - unrecognised type"
            );
            break;
    }

    return instruction;
}


std::string Context::get_store_instruction(Types type)
{
    std::string instruction;

    switch (type)
    {
        case Types::UNSIGNED_CHAR:
        case Types::CHAR:
            instruction = "sb";
            break;
        case Types::UNSIGNED_SHORT:
        case Types::SHORT:
            instruction = "sh";
            break;
        case Types::UNSIGNED_INT:
        case Types::INT:
        case Types::UNSIGNED_LONG:
        case Types::LONG:
            instruction = "sw";
            break;
        case Types::FLOAT:
            instruction = "fsw";
            break;
        case Types::DOUBLE:
        case Types::LONG_DOUBLE:
            instruction = "fsd";
            break;
        default:
            throw std::runtime_error(
                "Context::get_store_instruction() - unrecognised type"
            );
            break;
    }

    return instruction;
}


/**
 * Helper function to check if a mode is in mode_stack
*/
bool Context::has_mode(Mode mode) const
{
    // Take a copy of the stack
    std::stack<Mode> mode_stack_copy = mode_stack;

    // Keep popping
    while (!mode_stack_copy.empty())
    {
        if (mode_stack_copy.top() == mode)
        {
            return true;
        }
        mode_stack_copy.pop();
    }

    return false;
}
