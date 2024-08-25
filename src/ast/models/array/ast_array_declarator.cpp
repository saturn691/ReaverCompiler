#include <ast/models/array/ast_array_declarator.hpp>

namespace ast
{
  ArrayDeclarator::ArrayDeclarator(
      const Node *declarator)
      : declarator(std::unique_ptr<const Node>(declarator)),
        size(nullptr)
  {
  }

  ArrayDeclarator::ArrayDeclarator(
      const Node *declarator,
      const Node *size)
      : declarator(std::unique_ptr<const Node>(declarator)),
        size(std::unique_ptr<const Node>(size))
  {
  }

  void ArrayDeclarator::print(std::ostream &dst, int indent_level) const
  {
    declarator->print(dst, indent_level);
    dst << "[";
    size->print(dst, 0);
    dst << "]";
  }

  void ArrayDeclarator::lower(Context &context) const
  {
  }
}
