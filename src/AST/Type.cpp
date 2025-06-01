#include "AST/Type.hpp"

namespace AST
{

size_t BaseType::idProvider_ = 0;

BaseType::BaseType(TypeID tid) : tid_(tid)
{
    id_ = idProvider_++;
}

BaseType::BaseType(const BaseType &other)
{
    id_ = other.id_;
    tid_ = other.tid_;

    cvrQualifier_ = other.cvrQualifier_;
    functionSpecifier_ = other.functionSpecifier_;
    linkage_ = other.linkage_;
    storageDuration_ = other.storageDuration_;
}

size_t BaseType::getID() const noexcept
{
    return id_;
}

ArrayType::ArrayType(Ptr<BaseType> type, size_t size)
    : size_(size), PtrType(std::move(type)), BaseType(ArrayTyID)
{
    cvrQualifier_ = type_->cvrQualifier_;
    functionSpecifier_ = type_->functionSpecifier_;
    linkage_ = type_->linkage_;
    storageDuration_ = type_->storageDuration_;
}

ArrayType::ArrayType(const ArrayType &other)
    : size_(other.size_), PtrType(other), BaseType(other)
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
        // String literals can fit without the null terminator e.g. char a[3] =
        // "abc" (size 4);
        return size_ <= otherType->size_ + 1 && *type_ < *otherType->type_;
    }
    else if (auto otherType = dynamic_cast<const StructType *>(&other))
    {
        // Initializer arrays can fit in structs
        return true;
    }

    return false;
}

BasicType::BasicType(Types type) : type_(type), BaseType(BaseTypeID)
{
}

BasicType::BasicType(
    Types type,
    std::optional<CVRQualifier> cvrQualifier,
    std::optional<FunctionSpecifier> functionSpecifier,
    std::optional<Linkage> linkage,
    std::optional<StorageDuration> storageDuration)
    : type_(type), BaseType(BaseTypeID)
{
    cvrQualifier_ = cvrQualifier;
    functionSpecifier_ = functionSpecifier;
    linkage_ = linkage;
    storageDuration_ = storageDuration;
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

bool BasicType::isSigned() const noexcept
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
    : name_(std::move(name)), consts_(std::move(consts)), BaseType(EnumTypeID)
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
    : params_(std::move(params)), retType_(std::move(retType)), BaseType(FnTyID)
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
    return *params_ == *other.params_ && *retType_ == *other.retType_;
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

std::string FnType::getParamName(size_t i) const noexcept
{
    return params_->types_.at(i).first;
}

const BaseType *FnType::getParamType(size_t i) const noexcept
{
    return params_->types_.at(i).second.get();
}

ParamType::ParamType(Params types)
    : types_(std::move(types)), BaseType(ParamTyID)
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

size_t ParamType::size() const noexcept
{
    return types_.size();
}

const BaseType *ParamType::at(size_t i) const
{
    return types_[i].second.get();
}

Ptr<BaseType> ParamType::getMemberType(std::string name) const
{
    for (const auto &type : types_)
    {
        if (type.first == name)
        {
            return type.second->clone();
        }
    }

    return nullptr;
}

unsigned ParamType::getMemberIndex(std::string name) const
{

    for (size_t i = 0; i < types_.size(); ++i)
    {
        if (types_[i].first == name)
        {
            return i;
        }
    }

    return -1;
}

PtrType::PtrType(Ptr<BaseType> type) : type_(std::move(type)), BaseType(PtrTyID)
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

StructType::StructType(Type type, std::string name, size_t id)
    : type_(type), name_(std::move(name)), BaseType(StructTyID)
{
    // StructID only really useful in finding the correct scope
    if (id != -1)
    {
        id_ = id;
    }
}

StructType::StructType(const StructType &other)
    : type_(other.type_), name_(other.name_), BaseType(other)
{
}

bool StructType::operator==(const StructType &other) const
{
    // Both nullptr = OK
    return type_ == other.type_ && name_ == other.name_;
}

bool StructType::operator<(const BaseType &other) const
{
    return false;
}

std::string StructType::getName() const noexcept
{
    std::string prefix = type_ == Type::STRUCT ? "struct" : "union";
    return prefix + "." + name_;
}

} // namespace AST
