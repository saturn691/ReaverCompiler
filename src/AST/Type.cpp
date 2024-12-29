#include "AST/Type.hpp"

namespace AST
{

ArrayType::ArrayType(Ptr<BaseType> type, size_t size)
    : type_(std::move(type)), size_(size)
{
}

ArrayType::ArrayType(const ArrayType &other)
    : type_(other.type_->clone()), size_(other.size_)
{
}

bool ArrayType::operator==(const ArrayType &other) const
{
    return *type_ == *other.type_ && size_ == other.size_;
}
bool ArrayType::operator<(const BaseType &other) const
{
    // Can decay into a pointer, or a void pointer
    if (auto otherType = dynamic_cast<const PtrType *>(&other))
    {
        return PtrType(type_->clone()) <= *otherType;
    }
    return false;
}

BasicType::BasicType(Types type) : type_(type)
{
}
BasicType::BasicType(const BasicType &other) : type_(other.type_)
{
}

bool BasicType::operator==(const BasicType &other) const
{
    return type_ == other.type_;
}

bool BasicType::operator<(const BaseType &other) const
{
    if (auto otherType = dynamic_cast<const BasicType *>(&other))
    {
        return type_ < otherType->type_;
    }
    return false;
}

FnType::ParamType::ParamType(std::vector<Ptr<BaseType>> types)
    : types_(std::move(types))
{
}

FnType::ParamType::ParamType(const ParamType &other)
{
    for (const auto &type : other.types_)
    {
        types_.push_back(type->clone());
    }
}

bool FnType::ParamType::operator==(const ParamType &other) const
{
    if (types_.size() != other.types_.size())
    {
        return false;
    }

    for (size_t i = 0; i < types_.size(); ++i)
    {
        if (!(*types_[i] == *other.types_[i]))
        {
            return false;
        }
    }

    return true;
}

bool FnType::ParamType::operator<(const BaseType &other) const
{
    auto otherType = dynamic_cast<const ParamType *>(&other);
    if (!otherType)
    {
        return false;
    }

    if (types_.size() != otherType->types_.size())
    {
        return false;
    }

    for (size_t i = 0; i < types_.size(); ++i)
    {
        if (*types_[i] < *otherType->types_[i])
        {
            return true;
        }
    }

    return false;
}

size_t FnType::ParamType::size() const
{
    return types_.size();
}

const BaseType *FnType::ParamType::at(size_t i) const
{
    return types_[i].get();
}

FnType::FnType(Ptr<ParamType> params, Ptr<BaseType> retType)
    : params_(std::move(params)), retType_(std::move(retType))
{
}

FnType::FnType(const FnType &other)
    : params_(other.params_->cloneAsDerived()),
      retType_(other.retType_->clone())
{
}

bool FnType::operator==(const FnType &other) const
{
    return params_ == other.params_ && retType_ == other.retType_;
}

bool FnType::operator<(const BaseType &other) const
{
    // Can only decay into a function pointer with the same signature
    if (auto otherType = dynamic_cast<const PtrType *>(&other))
    {
        if (auto otherFnType =
                dynamic_cast<const FnType *>(otherType->type_.get()))
        {
            return *this == *otherFnType;
        }
    }

    return false;
}

PtrType::PtrType(Ptr<BaseType> type) : type_(std::move(type))
{
}
PtrType::PtrType(const PtrType &other) : type_(other.type_->clone())
{
}

bool PtrType::operator==(const PtrType &other) const
{
    return *type_ == *other.type_;
}

bool PtrType::operator<(const BaseType &other) const
{
    // Can only decay into a void pointer
    if (auto otherType = dynamic_cast<const PtrType *>(&other))
    {
        return *otherType->type_ == BasicType(Types::VOID);
    }
    return false;
}

} // namespace AST