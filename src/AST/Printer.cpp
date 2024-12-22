#include "AST/Printer.hpp"
#include "AST/Decl.hpp"
#include "AST/Expr.hpp"
#include "AST/Stmt.hpp"
#include "AST/Type.hpp"

#include <iostream>

namespace AST
{
/******************************************************************************
 *                          Declarations                                      *
 *****************************************************************************/

void Printer::visit(const DeclNode &node)
{
}

void Printer::visit(const FnDecl &node)
{
    node.decl_->accept(*this);
    os << "(";
    if (node.params_)
    {
        node.params_->accept(*this);
    }
    os << ")";
}

void Printer::visit(const FnDef &node)
{
    node.retType_->accept(*this);
    os << " ";
    node.decl_->accept(*this);
    os << std::endl;
    node.body_->accept(*this);
}

void Printer::visit(const ParamDecl &node)
{
    node.type_->accept(*this);
    os << " ";
    node.decl_->accept(*this);
}

void Printer::visit(const ParamList &node)
{
    for (const auto &param : node.nodes_)
    {
        std::visit([this](const auto &param) { param->accept(*this); }, param);
        if (param != node.nodes_.back())
        {
            os << ", ";
        }
    }
}

void Printer::visit(const TranslationUnit &node)
{
    for (const auto &decl : node.nodes_)
    {
        std::visit([this](const auto &decl) { decl->accept(*this); }, decl);
        os << std::endl;
    }
}

/******************************************************************************
 *                          Expressions                                       *
 *****************************************************************************/

void Printer::visit(const BinaryOp &node)
{
    node.lhs_->accept(*this);
    switch (node.op_)
    {
    case BinaryOp::Op::ADD:
        os << " + ";
        break;
    case BinaryOp::Op::SUB:
        os << " - ";
        break;
    case BinaryOp::Op::MUL:
        os << " * ";
        break;
    case BinaryOp::Op::DIV:
        os << " / ";
        break;
    case BinaryOp::Op::MOD:
        os << " % ";
        break;
    case BinaryOp::Op::AND:
        os << " & ";
        break;
    case BinaryOp::Op::OR:
        os << " | ";
        break;
    case BinaryOp::Op::XOR:
        os << " ^ ";
        break;
    case BinaryOp::Op::SHL:
        os << " << ";
        break;
    case BinaryOp::Op::SHR:
        os << " >> ";
        break;
    case BinaryOp::Op::EQ:
        os << " == ";
        break;
    case BinaryOp::Op::NE:
        os << " != ";
        break;
    case BinaryOp::Op::LT:
        os << " < ";
        break;
    case BinaryOp::Op::GT:
        os << " > ";
        break;
    case BinaryOp::Op::LE:
        os << " <= ";
        break;
    case BinaryOp::Op::GE:
        os << " >= ";
        break;
    case BinaryOp::Op::LAND:
        os << " && ";
        break;
    case BinaryOp::Op::LOR:
        os << " || ";
        break;
    }
    node.rhs_->accept(*this);
}

void Printer::visit(const Constant &node)
{
    os << node.value_;
}

void Printer::visit(const Identifier &node)
{
    os << node.name_;
}

/******************************************************************************
 *                          Statements *
 *****************************************************************************/

void Printer::visit(const BlockItemList &node)
{
    for (const auto &item : node.nodes_)
    {
        std::visit(
            [this](const auto &item)
            {
                os << getIndent();
                item->accept(*this);
                os << std::endl;
            },
            item);
    }
}

void Printer::visit(const CompoundStmt &node)
{
    os << getIndent() << "{" << std::endl;
    indentLevel++;
    node.nodes_->accept(*this);
    indentLevel--;
    os << getIndent() << "}";
}

void Printer::visit(const Return &node)
{
    os << "return ";
    node.expr_->get()->accept(*this);
    os << ";";
}

/******************************************************************************
 *                          Types *
 *****************************************************************************/

void Printer::visit(const BasicType &node)
{
    switch (node.type_)
    {
    case Types::VOID:
        os << "void";
        break;
    case Types::BOOL:
        os << "bool";
        break;
    case Types::UNSIGNED_CHAR:
        os << "unsigned char";
        break;
    case Types::CHAR:
        os << "char";
        break;
    case Types::UNSIGNED_SHORT:
        os << "unsigned short";
        break;
    case Types::SHORT:
        os << "short";
        break;
    case Types::UNSIGNED_INT:
        os << "unsigned int";
        break;
    case Types::INT:
        os << "int";
        break;
    case Types::UNSIGNED_LONG:
        os << "unsigned long";
        break;
    case Types::LONG:
        os << "long";
        break;
    case Types::FLOAT:
        os << "float";
        break;
    case Types::DOUBLE:
        os << "double";
        break;
    case Types::LONG_DOUBLE:
        os << "long double";
        break;
    case Types::COMPLEX:
        os << "_Complex";
        break;
    case Types::IMAGINARY:
        os << "_Imaginary";
        break;
    }
}

} // namespace AST