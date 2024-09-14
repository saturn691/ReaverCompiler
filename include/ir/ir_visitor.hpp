#pragma once

#include <ir/models/ir_basic_block.hpp>
#include <ir/models/ir_function.hpp>
#include <ir/models/ir_declaration.hpp>
#include <ir/models/ir_terminator.hpp>
#include <ir/models/ir_statement.hpp>

#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>

namespace ir
{
    /**
     * std::unique_ptr<> is not used, as memory management is taken care of by
     * the LLVM module.
     */
    class Visitor
    {
    public:
        virtual llvm::Value *codegen(const BinaryOp &expr) = 0;
        virtual llvm::Value *codegen(const Constant &expr) = 0;
        virtual llvm::Value *codegen(const Use &expr) = 0;
        virtual llvm::Value *codegen(const Cast &expr) = 0;

        virtual llvm::Value *codegen(const Assign &stmt) = 0;
        virtual std::string codegen(const Declaration &stmt) = 0;
    };
}
