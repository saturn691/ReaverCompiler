#ifndef ast_context_hpp
#define ast_context_hpp

#include <unordered_map>

/*
 *  Stores the states (e.g. variable lookup tables) and registers under usage.
*/
class Context
{
public:
    std::unordered_map<std::string, int> identifier_map;


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
};


#endif /* ast_context_hpp */
