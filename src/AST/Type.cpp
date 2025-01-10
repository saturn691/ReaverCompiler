#include "AST/Type.hpp"

namespace AST
{

BaseType::BaseType(const BaseType &other)
{
    cvrQualifier_ = other.cvrQualifier_;
    functionSpecifier_ = other.functionSpecifier_;
    linkage_ = other.linkage_;
    storageDuration_ = other.storageDuration_;
}

ArrayType::ArrayType(Ptr<BaseType> type, size_t size)
    : type_(std::move(type)), size_(size)
{
    cvrQualifier_ = type_->cvrQualifier_;
    functionSpecifier_ = type_->functionSpecifier_;
    linkage_ = type_->linkage_;
    storageDuration_ = type_->storageDuration_;
}

ArrayType::ArrayType(const ArrayType &other)
    : type_(other.type_->clone()), size_(other.size_), BaseType(other)
{
}

bool ArrayType::operator==(const ArrayType &other) const
{
    return *type_ == *other.type_ && size_ == other.size_;
}
bool ArrayType::operator<(const BaseType &other) const
{
    if (auto otherType = dynamic_cast<const PtrType *>(&other))
    {
        // Can decay into a pointer, or a void pointer
        return PtrType(type_->clone()) <= *otherType;
    }
    else if (auto otherType = dynamic_cast<const ArrayType *>(&other))
    {
        // Types can be casted as well
        // Arrays don't have to be fully initialized e.g. int a[5] = {1};
        return size_ <= otherType->size_ && *type_ < *otherType->type_;
    }

    return false;
}

BasicType::BasicType(Types type) : type_(type)
{
}
BasicType::BasicType(const BasicType &other)
    : type_(other.type_), BaseType(other)
{
}

bool BasicType::operator==(const BasicType &other) const
{
    return type_ == other.type_;
}

bool BasicType::operator<(const BaseType &other) const
{
    // Compatible with other type, also compatible with pointers (only if it is
    // 0, but give benefit of the doubt)
    return dynamic_cast<const BasicType *>(&other) ||
           dynamic_cast<const PtrType *>(&other);
}

bool BasicType::isSigned() const
{
    switch (type_)
    {
    case Types::BOOL:
    case Types::UNSIGNED_CHAR:
    case Types::UNSIGNED_SHORT:
    case Types::UNSIGNED_INT:
    case Types::UNSIGNED_LONG:
    case Types::UNSIGNED_LONG_LONG:
        return false;
    default:
        return true;
    }
}

EnumType::EnumType(std::string name, EnumConsts consts)
    : name_(std::move(name)), consts_(std::move(consts))
{
}

EnumType::EnumType(const EnumType &other)
    : name_(other.name_), consts_(other.consts_), BaseType(other)
{
}

bool EnumType::operator==(const EnumType &other) const
{
    return name_ == other.name_ && consts_ == other.consts_;
}

bool EnumType::operator<(const BaseType &other) const
{
    // Can only decay into an integer
    if (auto otherType = dynamic_cast<const BasicType *>(&other))
    {
        return otherType->type_ <= Types::INT;
    }
    return false;
}

FnType::FnType(Ptr<ParamType> params, Ptr<BaseType> retType)
    : params_(std::move(params)), retType_(std::move(retType))
{
    cvrQualifier_ = retType_->cvrQualifier_;
    functionSpecifier_ = retType_->functionSpecifier_;
    linkage_ = retType_->linkage_;
    storageDuration_ = retType_->storageDuration_;
}

FnType::FnType(const FnType &other)
    : params_(other.params_->cloneAsDerived()),
      retType_(other.retType_->clone()), BaseType(other)
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

ParamType::ParamType(Params types) : types_(std::move(types))
{
}

ParamType::ParamType(const ParamType &other) : BaseType(other)
{
    for (const auto &type : other.types_)
    {
        types_.push_back({type.first, type.second->clone()});
    }
}

bool ParamType::operator==(const ParamType &other) const
{
    if (types_.size() != other.types_.size())
    {
        return false;
    }

    for (size_t i = 0; i < types_.size(); ++i)
    {
        if (types_[i].first != other.types_[i].first ||
            *types_[i].second != *other.types_[i].second)
        {
            return false;
        }
    }

    return true;
}

bool ParamType::operator<(const BaseType &other) const
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
        if (*types_[i].second < *otherType->types_[i].second)
        {
            return true;
        }
    }

    return false;
}

size_t ParamType::size() const
{
    return types_.size();
}

const BaseType *ParamType::at(size_t i) const
{
    return types_[i].second.get();
}

PtrType::PtrType(Ptr<BaseType> type) : type_(std::move(type))
{
    cvrQualifier_ = type_->cvrQualifier_;
    functionSpecifier_ = type_->functionSpecifier_;
    linkage_ = type_->linkage_;
    storageDuration_ = type_->storageDuration_;
}

PtrType::PtrType(const PtrType &other)
    : type_(other.type_->clone()), BaseType(other)
{
}

bool PtrType::operator==(const PtrType &other) const
{
    return *type_ == *other.type_;
}

bool PtrType::operator<(const BaseType &other) const
{
    // C is uncivilized, we can cast any pointer to any pointer
    if (auto otherType = dynamic_cast<const PtrType *>(&other))
    {
        return true;
    }
    return false;
}

StructType::StructType(Type type, std::string name)
    : type_(type), name_(std::move(name))
{
}

StructType::StructType(Type type, std::string name, Ptr<ParamType> members)
    : type_(type), name_(std::move(name)), params_(std::move(members))
{
}

StructType::StructType(const StructType &other)
    : type_(other.type_), name_(other.name_), BaseType(other)
{
    if (other.params_)
    {
        params_ = other.params_->cloneAsDerived();
    }
}

bool StructType::operator==(const StructType &other) const
{
    return type_ == other.type_ && name_ == other.name_ &&
           params_ == other.params_;
}

bool StructType::operator<(const BaseType &other) const
{
    return false;
}

Ptr<BaseType> StructType::getMemberType(std::string name) const
{
    for (const auto &type : params_->types_)
    {
        if (type.first == name)
        {
            return type.second->clone();
        }
    }

    return nullptr;
}

unsigned int StructType::getMemberIndex(std::string name) const
{
    for (size_t i = 0; i < params_->types_.size(); ++i)
    {
        if (params_->types_[i].first == name)
        {
            return i;
        }
    }

    return -1;
}

} // namespace AST