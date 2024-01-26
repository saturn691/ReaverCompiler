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
    )
{}


std::string Context::allocate_register(Types type)
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


std::string Context::allocate_arg_register(Types type)
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


void Context::deallocate_register(std::string register_name)
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


void Context::push_registers(std::ostream& dst)
{
    std::string indent(AST_PRINT_INDENT_SPACES, ' ');

    dst << "# Pushing registers onto stack (if any)" << std::endl;

    // Search temporary registers (fa0-fa7)
    for (int i = 0; i <= 11; ++i)
    {
        std::string register_name = "ft" + std::to_string(i);
        int reg_index = register_map_f.at(register_name);
        if (registers_f[reg_index] == 1)
        {
            registers_f[reg_index] = 0;
            // 32-bit registers
            int stack_loc = allocate_stack(
                Types::DOUBLE,
                "!" + register_name
            );
            dst << indent << "fsw " << register_name
                << ", " << stack_loc << "(s0)" << std::endl;
        }
    }

    // Search temporary registers (a0-a7)
    for (int i = 0; i <= 6; ++i)
    {
        std::string register_name = "t" + std::to_string(i);
        int reg_index = register_map.at(register_name);
        if (registers[reg_index] == 1)
        {
            registers[reg_index] = 0;
            // 32-bit registers
            int stack_loc = allocate_stack(
                Types::LONG,
                "!" + register_name
            );
            dst << indent << "sw " << register_name
                << ", " << stack_loc << "(s0)" << std::endl;
        }
    }
}


void Context::pop_registers(std::ostream& dst)
{
    std::string indent(AST_PRINT_INDENT_SPACES, ' ');
    std::vector<std::string> to_erase;
    dst << "# Popping registers from stack (if any)" << std::endl;

    for (const auto& pair : identifier_map)
    {
        if (pair.first[0] == '!')
        {
            std::string dest_reg = pair.first.substr(1);
            int stack_loc = pair.second.stack_location;
            Types type = pair.second.type;

            switch (type)
            {
                case Types::DOUBLE:
                    dst << indent << "flw " << dest_reg << ", "
                        << stack_loc << "(s0)" << std::endl;
                    break;
                default:
                    dst << indent << "lw " << dest_reg << ", "
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
        identifier_map.erase(e);
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


void Context::end_stack(std::ostream& dst)
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


int Context::allocate_stack(Types type, std::string id)
{
    unsigned int bytes = type_size_map.at(type);
    int stack_loc = push_stack(bytes);

    if (!id.empty())
    {
        identifier_map[id] = {stack_loc, type};
    }
    else
    {
        throw std::runtime_error(
            "Context::allocate_stack() - id is empty"
        );
    }

    return stack_loc;
}

int Context::allocate_array_stack(Types type, int size, std::string id)
{
    unsigned int bytes = type_size_map.at(type)*size;
    int stack_loc = push_stack(bytes);

    if (!id.empty())
    {
        identifier_map[id] = {stack_loc, type};
    }
    else
    {
        throw std::runtime_error(
            "Context::allocate_array_stack() - id is empty"
        );
    }

    return stack_loc;
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
    unsigned int id = tag_next_id;
    tag_next_id++;

    std::string tag = prefix + std::to_string(id);

    return tag;
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

    for (const auto& [id, val] : string_map)
    {
        // This is what GCC does.
        dst << ".align 2" << std::endl;
        dst << "." << id << ":" << std::endl;

        // Necessary for floating point representations.
        dst << indent << ".string " << val << std::endl;
    }
}


void Context::add_function_declaration(std::string id)
{
    FunctionVariable function;
    function.stack_location = -1;
    function.type = current_declaration_type->get_type();
    function.parameter_types.clear();

    identifier_map.insert({id, function});
    current_id = id;
}


void Context::add_function_declaration_type(Types type)
{
    identifier_map.at(current_id).parameter_types.push_back(type);
}

void Context::set_is_pointer(bool is_pointer, std::string id)
{
    identifier_map.at(id).is_pointer = is_pointer;
}

bool Context::get_is_pointer(std::string id) const
{
    return identifier_map.at(id).is_pointer;
}

Types Context::get_type(std::string id) const
{
    // First, try to find the id in the identifier_map
    auto identifier_it = identifier_map.find(id);
    if (identifier_it != identifier_map.end()) {
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
    return identifier_map.at(id).stack_location;
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

    for (const auto& [key, value] : identifier_map)
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
