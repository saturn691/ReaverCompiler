#pragma once

/**
 * Declarations are used to declare variables, functions, typedef, enums etc.
 * They include the type information and (a list of) (init) declarators.
 * They include raw declarations and definitions.
 *
 * For reference: https://en.cppreference.com/w/c/language/declarations
 */

#include "ir/models/ir_basic_block.hpp"
#include <memory>

#include <ast/models/ast_node.hpp>
#include <ast/models/ast_type.hpp>
#include <ast/models/ast_statement.hpp>

#include <ir/models/ir_ir.hpp>

namespace ast
{
    // Forward declarations
    class CompoundStatement;
    class FunctionDeclarator;
    class FunctionParamList;
    class FunctionParam;
    class Expression;

    /**
     * Base class for all declarations
     */
    class Declaration : public Node
    {
    public:
        virtual ~Declaration() = default;

        virtual void print(std::ostream &dst, int indent_level) const override = 0;
    };

    /**
     * Base class for all declarators. Declarators are a part of a declaration
     * that specifies the name of the variable, function, etc. and can include
     * additional information such as pointers and arrays.
     */
    class Declarator : public Node
    {
    public:
        virtual ~Declarator() = default;

        virtual void print(std::ostream &dst, int indent_level) const override = 0;

        virtual std::string get_id() const = 0;
    };

    /**
     * A function definition, including the body.
     * e.g. `int foo(int x) { return x + 1; }`
     */
    class FunctionDefinition : public Declaration
    {
    public:
        FunctionDefinition(
            const Type *specifiers,
            const FunctionDeclarator *declarator,
            const CompoundStatement *statement);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(Context &context, std::unique_ptr<ir::IR> &ir) const;

    private:
        std::unique_ptr<const Type> specifiers;
        std::unique_ptr<const FunctionDeclarator> declarator;
        std::unique_ptr<const CompoundStatement> statement;
    };

    /**
     * A function declaration, without the body.
     * e.g. `int foo(int x)`
     */
    class FunctionDeclarator : public Declarator
    {
    public:
        FunctionDeclarator(const Declarator *decl);

        FunctionDeclarator(
            const Declarator *decl,
            const FunctionParamList *params);

        void print(std::ostream &dst, int indent_level) const override;

        std::string get_id() const override;

        std::vector<ir::Declaration> lower() const;

    private:
        std::unique_ptr<const Declarator> decl;
        std::unique_ptr<const FunctionParamList> params;
    };

    /**
     * A function parameter list, used in function declarations and definitions.
     * e.g. `int x, float y`
     */
    class FunctionParamList : public NodeList<FunctionParam>
    {
    public:
        using NodeList::NodeList;

        void print(std::ostream &dst, int indent_level) const override;

        std::vector<ir::Declaration> lower() const;
    };

    /**
     * A function parameter, used in function declarations and definitions.
     * e.g. `int x`
     */
    class FunctionParam : public Node
    {
    public:
        FunctionParam(const Type *type);

        FunctionParam(const Type *type, const Declarator *decl);

        void print(
            std::ostream &dst,
            [[maybe_unused]] int indent_level) const override;

        ir::Declaration lower() const;

    private:
        std::unique_ptr<const Type> type;
        std::unique_ptr<const Declarator> decl;
    };

    /**
     * An init declarator, used in variable declarations.
     * e.g. `x = 0`
     */
    class InitDeclarator : public Declarator
    {
    public:
        InitDeclarator(const Declarator *decl);

        // The init expression must be an rvalue
        InitDeclarator(const Declarator *decl, const Expression *init);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(
            Context &context,
            const std::unique_ptr<ir::BasicBlock>& bb,
            const std::unique_ptr<const Type>& ty) const;

        std::string get_id() const override;

    private:
        std::unique_ptr<const Declarator> decl;
        std::unique_ptr<const Expression> init;
    };

    /**
     * A list of init declarators.
     * e.g. `int x = 0, y = 1`
     */
    class InitDeclaratorList : public NodeList<InitDeclarator>
    {
    public:
        using NodeList::NodeList;

        void lower(
            Context &context,
            const std::unique_ptr<ir::BasicBlock>& bb,
            const std::unique_ptr<const Type>& ty) const;
    };

    /**
     * A declaration node, refers to the declaration in the parser with a
     * semicolon.
     */
    class DeclarationNode : public Declaration
    {
    public:
        DeclarationNode(const Type *specifiers);

        DeclarationNode(
            const Type *specifiers,
            const InitDeclaratorList *decls);

        void print(std::ostream &dst, int indent_level) const override;

        void lower(
            Context &context,
            std::unique_ptr<ir::IR> &ir) const;

        void lower(
            Context& context,
            const std::unique_ptr<ir::BasicBlock> &bb) const;

    private:
        std::unique_ptr<const Type> specifiers;
        std::unique_ptr<const InitDeclaratorList> decls;
    };

    /**
     * A list of declarations.
     */
    class DeclarationList : public NodeList<DeclarationNode>
    {
    public:
        using NodeList::NodeList;

        void lower(
            Context& context,
            const std::unique_ptr<ir::BasicBlock> &bb) const;
    };
}
