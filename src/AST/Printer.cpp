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

void Printer::visit(const ArrayDecl &node)
{
    node.decl_->accept(*this);
    os << "[";
    node.size_->accept(*this);
    os << "]";
}

void Printer::visit(const DeclNode &node)
{
    node.type_->accept(*this);
    if (node.initDeclList_)
    {
        os << " ";
        node.initDeclList_->accept(*this);
    }
    os << ";";
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

void Printer::visit(const InitDecl &node)
{
    node.decl_->accept(*this);
    if (node.expr_)
    {
        os << " = ";
        node.expr_->accept(*this);
    }
}

void Printer::visit(const InitDeclList &node)
{
    for (const auto &initDecl : node.nodes_)
    {
        std::visit(
            [this](const auto &initDecl) { initDecl->accept(*this); },
            initDecl);
        if (initDecl != node.nodes_.back())
        {
            os << ", ";
        }
    }
}

void Printer::visit(const ParamDecl &node)
{
    node.type_->accept(*this);
    if (node.decl_)
    {
        os << " ";
        node.decl_->accept(*this);
    }
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

void Printer::visit(const PtrDecl &node)
{
    node.ptr_->accept(*this);
    node.decl_->accept(*this);
}

void Printer::visit(const PtrNode &node)
{
    os << "*";
}

void Printer::visit(const TranslationUnit &node)
{
    for (const auto &decl : node.nodes_)
    {
        std::visit([this](const auto &decl) { decl->accept(*this); }, decl);
        os << std::endl << std::endl;
    }
}

/******************************************************************************
 *                          Expressions                                       *
 *****************************************************************************/

void Printer::visit(const ArrayAccess &node)
{
    node.arr_->accept(*this);
    os << "[";
    node.index_->accept(*this);
    os << "]";
}

void Printer::visit(const Assignment &node)
{
    node.lhs_->accept(*this);
    switch (node.op_)
    {
    case Assignment::Op::ASSIGN:
        os << " = ";
        break;
    case Assignment::Op::MUL_ASSIGN:
        os << " *= ";
        break;
    case Assignment::Op::DIV_ASSIGN:
        os << " /= ";
        break;
    case Assignment::Op::MOD_ASSIGN:
        os << " %= ";
        break;
    case Assignment::Op::ADD_ASSIGN:
        os << " += ";
        break;
    case Assignment::Op::SUB_ASSIGN:
        os << " -= ";
        break;
    case Assignment::Op::LEFT_ASSIGN:
        os << " <<= ";
        break;
    case Assignment::Op::RIGHT_ASSIGN:
        os << " >>= ";
        break;
    case Assignment::Op::AND_ASSIGN:
        os << " &= ";
        break;
    case Assignment::Op::XOR_ASSIGN:
        os << " ^= ";
        break;
    case Assignment::Op::OR_ASSIGN:
        os << " |= ";
        break;
    }
    node.rhs_->accept(*this);
}

void Printer::visit(const ArgExprList &node)
{
    for (const auto &arg : node.nodes_)
    {
        std::visit([this](const auto &arg) { arg->accept(*this); }, arg);
        if (arg != node.nodes_.back())
        {
            os << ", ";
        }
    }
}

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

void Printer::visit(const FnCall &node)
{
    node.fn_->accept(*this);
    os << "(";
    if (node.args_)
    {
        node.args_->accept(*this);
    }
    os << ")";
}

void Printer::visit(const Identifier &node)
{
    os << node.name_;
}

void Printer::visit(const Paren &node)
{
    os << "(";
    node.expr_->accept(*this);
    os << ")";
}

void Printer::visit(const SizeOf &node)
{
    os << "sizeof";
    if (node.expr_)
    {
        // Parenthesis will be added by the expression
        node.expr_->accept(*this);
    }
    else
    {
        os << "(";
        node.type_->accept(*this);
        os << ")";
    }
}

void Printer::visit(const StringLiteral &node)
{
    os << node.originalValue_;
}

void Printer::visit(const UnaryOp &node)
{
    switch (node.op_)
    {
    case UnaryOp::Op::ADDR:
        os << "&";
        break;
    case UnaryOp::Op::DEREF:
        os << "*";
        break;
    case UnaryOp::Op::PLUS:
        os << "+";
        break;
    case UnaryOp::Op::MINUS:
        os << "-";
        break;
    case UnaryOp::Op::NOT:
        os << "~";
        break;
    case UnaryOp::Op::LNOT:
        os << "!";
        break;
    case UnaryOp::Op::POST_DEC:
        node.expr_->accept(*this);
        os << "--";
        return;
    case UnaryOp::Op::POST_INC:
        node.expr_->accept(*this);
        os << "++";
        return;
    case UnaryOp::Op::PRE_DEC:
        os << "--";
        break;
    case UnaryOp::Op::PRE_INC:
        os << "++";
        break;
    }
    node.expr_->accept(*this);
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
    if (node.nodes_)
    {
        indentLevel++;
        node.nodes_->accept(*this);
        indentLevel--;
    }
    os << getIndent() << "}";
}

void Printer::visit(const ExprStmt &node)
{
    node.expr_->accept(*this);
    os << ";";
}

void Printer::visit(const For &node)
{
    os << "for (";
    if (std::holds_alternative<Ptr<DeclNode>>(node.init_))
    {
        std::get<Ptr<DeclNode>>(node.init_)->accept(*this);
    }
    else
    {
        std::get<Ptr<Stmt>>(node.init_)->accept(*this);
    }
    os << " ";
    node.cond_->accept(*this);

    if (node.expr_)
    {
        os << " ";
        node.expr_->accept(*this);
    }

    os << ")" << std::endl;
    node.body_->accept(*this);
}

void Printer::visit(const IfElse &node)
{
    os << "if (";
    node.cond_->accept(*this);
    os << ")" << std::endl;
    node.thenStmt_->accept(*this);
    if (node.elseStmt_)
    {
        os << std::endl;
        os << getIndent() << "else" << std::endl;
        node.elseStmt_->accept(*this);
    }
}

void Printer::visit(const Return &node)
{
    os << "return ";
    if (node.expr_)
    {
        node.expr_->accept(*this);
    }
    os << ";";
}

void Printer::visit(const While &node)
{
    os << "while (";
    node.cond_->accept(*this);
    os << ")" << std::endl;
    node.body_->accept(*this);
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