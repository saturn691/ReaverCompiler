#pragma once

#include <string>

#include "AST/Decl.hpp"
#include "AST/Expr.hpp"
#include "AST/Node.hpp"
#include "AST/Printer.hpp"
#include "AST/Stmt.hpp"
#include "AST/Type.hpp"
#include "AST/Visitor.hpp"

namespace AST
{
extern const TranslationUnit *parseAST(const std::string &filename);
} // namespace AST