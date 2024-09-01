#include <ir/models/ir_basic_block.hpp>

#include <ir/models/ir_terminator.hpp>

namespace ir
{
    BasicBlock::BasicBlock()
        : statements(),
          terminator(nullptr)
    {
    }

    BasicBlock::BasicBlock(BasicBlock &&other) noexcept
        : statements(),
          terminator(nullptr)
    {
        *this = std::move(other);
    }

    BasicBlock &BasicBlock::operator=(BasicBlock &&other) noexcept
    {
        statements = std::move(other.statements);
        terminator = std::move(other.terminator);

        other.statements.clear();
        other.terminator = nullptr;

        return *this;
    }

    void BasicBlock::print(std::ostream &dst, int indent_level) const
    {
        for (const auto &stmt : statements)
        {
            stmt->print(dst, indent_level + 1);
        }
        if (terminator != nullptr)
        {
            terminator->print(dst, indent_level + 1);
        }
    }
}
