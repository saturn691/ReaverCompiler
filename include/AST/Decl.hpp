#pragma once

#include "AST/Node.hpp"
#include "AST/Type.hpp"

namespace AST
{
// Forward declarations
class CompoundStmt;
class InitDeclList;
class PtrNode;

/**
 * Base class for declarations (including definitions and declarators)
 */
class Decl : public virtual BaseNode
{
public:
    virtual ~Decl() = default;
    virtual std::string getID() const = 0;
};

/**
 * Base class for types in the AST.
 */
class TypeDecl : public Decl
{
public:
    virtual ~TypeDecl() = default;

    // Gets the Type object associated with the declaration
    virtual Ptr<BaseType> getType() const = 0;
};

/**
 * Array declaration
 * e.g. `a[10]`
 */
class ArrayDecl final : public Node<ArrayDecl>, public Decl
{
public:
    ArrayDecl(const Decl *decl, const Expr *size);

    std::string getID() const override;

    Ptr<Decl> decl_;
    Ptr<Expr> size_;
};

/**
 * Basic type node
 * e.g. `int`
 */
class BasicTypeDecl final : public Node<BasicTypeDecl>, public TypeDecl
{
public:
    BasicTypeDecl(Types type) : type_(type)
    {
    }

    std::string getID() const override
    {
        return "";
    }

    Ptr<BaseType> getType() const override
    {
        return std::make_unique<BasicType>(type_);
    }

    Types type_;
};

/**
 * Declaration node
 * e.g. `int a = 1;`
 */
class DeclNode final : public Node<DeclNode>, public Decl
{
public:
    DeclNode(const TypeDecl *type);
    DeclNode(const TypeDecl *type, const InitDeclList *decl);

    std::string getID() const override
    {
        return "";
    }

    std::vector<std::string> getIDs() const;

    Ptr<TypeDecl> type_;
    Ptr<InitDeclList> initDeclList_; // Optional
};

/**
 * Defined type by typedef
 * e.g. `typedef int myInt;`
 */
class DefinedTypeDecl final : public Node<DefinedTypeDecl>, public TypeDecl
{
public:
    DefinedTypeDecl(std::string name) : name_(std::move(name))
    {
    }

    std::string getID() const override
    {
        return name_;
    }

    Ptr<BaseType> getType() const override
    {
        return nullptr;
    }

    std::string name_;
};

/**
 * Enum declaration or instance
 * e.g. `enum E { A, B, C }` or `enum E`
 */
class Enum final : public Node<Enum>, public TypeDecl
{
public:
    // 1. Definition
    Enum(std::string name, const EnumMemberList *members)
        : name_(std::move(name)), members_(members)
    {
    }
    // 2. Anonymous definition
    Enum(const EnumMemberList *members) : members_(members)
    {
    }
    // 3. Instance
    Enum(std::string name) : name_(std::move(name))
    {
    }

    std::string getID() const override
    {
        return "enum " + name_;
    }

    Ptr<BaseType> getType() const override
    {
        return std::make_unique<EnumType>(name_, EnumConsts());
    }

    std::string name_;            // Optional
    Ptr<EnumMemberList> members_; // Optional
};

/**
 * Enum member
 * e.g. `A = 1`
 */
class EnumMember final : public Node<EnumMember>, public Decl
{
public:
    EnumMember(std::string id) : id_(std::move(id))
    {
    }

    EnumMember(std::string id, const Expr *expr)
        : id_(std::move(id)), expr_(expr)
    {
    }

    std::string getID() const override
    {
        return id_;
    }

    std::string id_;
    Ptr<Expr> expr_; // Optional
};

/**
 * Enum member list
 * e.g. `A = 1, B = 2, C = 3`
 */
class EnumMemberList final : public NodeList<EnumMember>,
                             public Node<EnumMemberList>
{
public:
    using NodeList::NodeList;
};

/**
 * Function declarator
 * e.g. `foo(int a)
 */
class FnDecl final : public Node<FnDecl>, public Decl
{
public:
    FnDecl(const Decl *decl)
        : decl_(decl), params_(std::make_unique<ParamList>())
    {
    }

    FnDecl(const Decl *decl, const ParamList *params)
        : decl_(decl), params_(params)
    {
    }

    std::string getID() const override
    {
        return decl_->getID();
    }

    Ptr<Decl> decl_;
    Ptr<ParamList> params_;
};

/**
 * Function definition
 * e.g. `int foo(int a) { return a; }`
 */
class FnDef final : public Node<FnDef>, public Decl
{
public:
    FnDef(const TypeDecl *retType, const Decl *decl, const CompoundStmt *body);

    std::string getID() const override;

    Ptr<TypeDecl> retType_;
    Ptr<Decl> decl_;
    Ptr<CompoundStmt> body_;
};

/**
 * Initializer declarator
 * e.g. `a = 1`
 */
class InitDecl final : public Node<InitDecl>, public Decl
{
public:
    InitDecl(const Decl *decl);
    InitDecl(const Decl *decl, const Expr *expr);

    std::string getID() const override;

    Ptr<Decl> decl_;
    Ptr<Expr> expr_;
};

/**
 * Initializer declarator list
 * e.g. `int a = 1, b = 2`
 */
class InitDeclList final : public NodeList<InitDecl>, public Node<InitDeclList>
{
public:
    using NodeList::NodeList;
};

/**
 * Parameter declaration
 * e.g. `int a`
 */
class ParamDecl final : public Node<ParamDecl>, public Decl
{
public:
    ParamDecl(const TypeDecl *type) : type_(type)
    {
    }

    ParamDecl(const TypeDecl *type, const Decl *decl) : type_(type), decl_(decl)
    {
    }

    std::string getID() const override
    {
        if (decl_)
        {
            return decl_->getID();
        }

        return "";
    }

    Ptr<TypeDecl> type_;
    Ptr<Decl> decl_; // Optional
};

/**
 * Parameter list
 * e.g. `int a, int b`
 */
class ParamList final : public NodeList<ParamDecl>, public Node<ParamList>
{
public:
    using NodeList::NodeList;
};

/**
 * Pointer declarator
 * e.g. `int *a`
 */
class PtrDecl final : public Node<PtrDecl>, public Decl
{
public:
    PtrDecl(const PtrNode *ptr, const Decl *decl) : ptr_(ptr), decl_(decl)
    {
    }

    std::string getID() const override
    {
        return decl_->getID();
    }

    Ptr<PtrNode> ptr_;
    Ptr<Decl> decl_;
};

/**
 * Pointer node
 * e.g. `*` or `const*`
 */
class PtrNode final : public Node<PtrNode>
{
public:
    PtrNode() = default;
    PtrNode(const PtrNode *ptr) : ptr_(ptr)
    {
    }

    unsigned int getPointerLevel() const
    {
        return ptr_ ? ptr_->getPointerLevel() + 1 : 1;
    }

    Ptr<PtrNode> ptr_;
};

/**
 * Struct declaration, including definitions and instances
 */
class Struct final : public Node<Struct>, public TypeDecl
{
public:
    using Type = StructType::Type;

    // 1. Definition
    Struct(Type type, std::string name, const StructMemberList *members);
    // 2. Anonymous declaration
    Struct(Type type, const StructMemberList *members);
    // 3. Instance
    Struct(Type type, std::string name);

    std::string getID() const override
    {
        std::string prefix = (type_ == Type::STRUCT) ? "struct " : "union ";
        return prefix + name_;
    }

    Ptr<BaseType> getType() const override
    {
        return std::make_unique<StructType>(type_, name_);
    };

    Type type_;
    std::string name_;              // Optional
    Ptr<StructMemberList> members_; // Optional
};

/**
 * Struct declarator
 * e.g. `a`
 */
class StructDecl final : public Node<StructDecl>, public Decl
{
public:
    StructDecl(const Decl *decl) : decl_(decl)
    {
    }

    std::string getID() const override
    {
        return decl_->getID();
    }

    Ptr<Decl> decl_;
};

/**
 * List of struct declarators
 * e.g. `a, b, c`
 */
class StructDeclList final : public NodeList<StructDecl>,
                             public Node<StructDeclList>
{
public:
    using NodeList::NodeList;
};

/**
 * Struct member
 * e.g. `int a, b;`
 */
class StructMember final : public Node<StructMember>
{
public:
    StructMember(const TypeDecl *type, const StructDeclList *declList)
        : type_(type), declList_(declList)
    {
    }

    Ptr<TypeDecl> type_;
    Ptr<StructDeclList> declList_;
};

/**
 * List of struct members
 * e.g. `int a, b; float c;`
 */
class StructMemberList final : public NodeList<StructMember>,
                               public Node<StructMemberList>
{
public:
    using NodeList::NodeList;
};

/**
 * Translation unit. Entry point of the program.
 * e.g. `int main() { return 0; }`
 */
class TranslationUnit final : public NodeList<DeclNode, FnDef>,
                              public Node<TranslationUnit>
{
public:
    using NodeList::NodeList;
};

/**
 * Typedef declaration
 * e.g. `typedef int`
 */
class Typedef final : public Node<Typedef>, public TypeDecl
{
public:
    Typedef(const TypeDecl *type) : type_(type)
    {
    }

    std::string getID() const override
    {
        return "";
    }

    Ptr<BaseType> getType() const override
    {
        return type_->getType();
    }

    Ptr<TypeDecl> type_;
};

} // namespace AST