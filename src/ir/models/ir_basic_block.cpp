#include <ir/models/ir_basic_block.hpp>

namespace ir
{
    BasicBlock::BasicBlock()
        : terminator(nullptr)
    {
    }

    BasicBlock::BasicBlock(const BasicBlock &other)
        : statements(),
          terminator()
    {
    }

    void BasicBlock::print(std::ostream &dst, int indent_level) const
    {
    }
}
