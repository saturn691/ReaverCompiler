#ifndef ast_context_hpp
#define ast_context_hpp

#include <unordered_map>

#include "ast_types.hpp"


/*
 *  Stores the states (e.g. variable lookup tables) and registers under usage.
*/
class Context
{
public:
    std::unordered_map<std::string, int> identifier_value_map;
    std::unordered_map<std::string, Types> identifier_type_map;

    std::string allocate_register(Types type)
    {
        switch (type)
        {
            case Types::INT:
            case Types::UNSIGNED_INT:
                // Search temporary registers
                for (int i = 0; i <= 2; ++i)
                {
                    if (registers[i + 5] == 0)
                    {
                        registers[i + 5] = 1;
                        return "t" + std::to_string(i);
                    }
                }
                for (int i = 0; i <= 3; ++i)
                {
                    if (registers[i + 28] == 0)
                    {
                        registers[i + 28] = 1;
                        return "t" + std::to_string(i + 3);
                    }
                }
                break;

            default:
                throw std::runtime_error(
                    "Context::allocate_register() - unrecognised type"
                );
        }
    }

private:
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
};


#endif /* ast_context_hpp */
