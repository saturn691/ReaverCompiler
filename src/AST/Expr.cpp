#include "AST/Expr.hpp"
#include "AST/Type.hpp"

#include <cmath>

namespace AST
{

EvalType::EvalType() : value(std::monostate{})
{
}

EvalType::EvalType(double value) : value(value)
{
}

EvalType::EvalType(uint64_t value) : value(value)
{
}

EvalType::EvalType(int64_t value) : value(value)
{
}

EvalType::EvalType(std::string value) : value(std::move(value))
{
}

std::optional<double> EvalType::getDouble() const
{
    if (std::holds_alternative<double>(value))
    {
        return std::get<double>(value);
    }
    else if (std::holds_alternative<int64_t>(value))
    {
        return static_cast<double>(std::get<int64_t>(value));
    }
    else if (std::holds_alternative<uint64_t>(value))
    {
        return static_cast<double>(std::get<uint64_t>(value));
    }

    return std::nullopt;
}

std::optional<uint64_t> EvalType::getUInt() const
{
    if (std::holds_alternative<uint64_t>(value))
    {
        return std::get<uint64_t>(value);
    }
    else if (std::holds_alternative<int64_t>(value))
    {
        return static_cast<uint64_t>(std::get<int64_t>(value));
    }
    else if (std::holds_alternative<double>(value))
    {
        return static_cast<uint64_t>(std::get<double>(value));
    }

    return std::nullopt;
}

std::optional<int64_t> EvalType::getInt() const
{
    if (std::holds_alternative<int64_t>(value))
    {
        return std::get<int64_t>(value);
    }
    else if (std::holds_alternative<uint64_t>(value))
    {
        return static_cast<int64_t>(std::get<uint64_t>(value));
    }
    else if (std::holds_alternative<double>(value))
    {
        return static_cast<int64_t>(std::get<double>(value));
    }

    return std::nullopt;
}

std::optional<std::string> EvalType::getString() const
{
    if (std::holds_alternative<std::string>(value))
    {
        return std::get<std::string>(value);
    }

    return std::nullopt;
}

EvalType::operator bool() const
{
    return !std::holds_alternative<std::monostate>(value);
}

Types EvalType::getType() const
{
    if (std::holds_alternative<double>(value))
    {
        return Types::DOUBLE;
    }
    else if (std::holds_alternative<uint64_t>(value))
    {
        return Types::UNSIGNED_LONG;
    }
    else if (std::holds_alternative<int64_t>(value))
    {
        return Types::LONG;
    }

    return Types::VOID;
}

BinaryOp::BinaryOp(const Expr *lhs, const Expr *rhs, Op op)
    : lhs_(lhs), rhs_(rhs), op_(op)
{
}

EvalType BinaryOp::eval() const
{
    auto l = lhs_->eval();
    auto r = rhs_->eval();

    if (!l || !r || l.is<std::string>() || r.is<std::string>())
    {
        return {};
    }

    if (l.is<double>() || r.is<double>())
    {
        double lhs = *l.getDouble();
        double rhs = *r.getDouble();

        switch (op_)
        {
        case Op::ADD:
            return lhs + rhs;
        case Op::SUB:
            return lhs - rhs;
        case Op::MUL:
            return lhs * rhs;
        case Op::DIV:
            return lhs / rhs;
        case Op::MOD:
        case Op::AND:
        case Op::OR:
        case Op::XOR:
        case Op::SHL:
        case Op::SHR:
            throw std::runtime_error("Operation not allowed on floating point");
        case Op::EQ:
            return static_cast<uint64_t>(lhs == rhs);
        case Op::NE:
            return static_cast<uint64_t>(lhs != rhs);
        case Op::LT:
            return static_cast<uint64_t>(lhs < rhs);
        case Op::GT:
            return static_cast<uint64_t>(lhs > rhs);
        case Op::LE:
            return static_cast<uint64_t>(lhs <= rhs);
        case Op::GE:
            return static_cast<uint64_t>(lhs >= rhs);
        case Op::LAND:
            return static_cast<uint64_t>(lhs && rhs);
        case Op::LOR:
            return static_cast<uint64_t>(lhs || rhs);
        }
    }

    if (l.is<uint64_t>() || r.is<uint64_t>())
    {
        uint64_t lhs = *l.getUInt();
        uint64_t rhs = *r.getUInt();

        switch (op_)
        {
        case Op::ADD:
            return lhs + rhs;
        case Op::SUB:
            return lhs - rhs;
        case Op::MUL:
            return lhs * rhs;
        case Op::DIV:
            return lhs / rhs;
        case Op::MOD:
            return lhs % rhs;
        case Op::AND:
            return lhs & rhs;
        case Op::OR:
            return lhs | rhs;
        case Op::XOR:
            return lhs ^ rhs;
        case Op::SHL:
            return lhs << rhs;
        case Op::SHR:
            return lhs >> rhs;
        case Op::EQ:
            return static_cast<uint64_t>(lhs == rhs);
        case Op::NE:
            return static_cast<uint64_t>(lhs != rhs);
        case Op::LT:
            return static_cast<uint64_t>(lhs < rhs);
        case Op::GT:
            return static_cast<uint64_t>(lhs > rhs);
        case Op::LE:
            return static_cast<uint64_t>(lhs <= rhs);
        case Op::GE:
            return static_cast<uint64_t>(lhs >= rhs);
        case Op::LAND:
            return static_cast<uint64_t>(lhs && rhs);
        case Op::LOR:
            return static_cast<uint64_t>(lhs || rhs);
        }
    }

    int64_t lhs = *l.getInt();
    int64_t rhs = *r.getInt();

    switch (op_)
    {
    case Op::ADD:
        return lhs + rhs;
    case Op::SUB:
        return lhs - rhs;
    case Op::MUL:
        return lhs * rhs;
    case Op::DIV:
        return lhs / rhs;
    case Op::MOD:
        return lhs % rhs;
    case Op::AND:
        return lhs & rhs;
    case Op::OR:
        return lhs | rhs;
    case Op::XOR:
        return lhs ^ rhs;
    case Op::SHL:
        return lhs << rhs;
    case Op::SHR:
        return lhs >> rhs;
    case Op::EQ:
        return static_cast<uint64_t>(lhs == rhs);
    case Op::NE:
        return static_cast<uint64_t>(lhs != rhs);
    case Op::LT:
        return static_cast<uint64_t>(lhs < rhs);
    case Op::GT:
        return static_cast<uint64_t>(lhs > rhs);
    case Op::LE:
        return static_cast<uint64_t>(lhs <= rhs);
    case Op::GE:
        return static_cast<uint64_t>(lhs >= rhs);
    case Op::LAND:
        return static_cast<uint64_t>(lhs && rhs);
    case Op::LOR:
        return static_cast<uint64_t>(lhs || rhs);
    }
}

Constant::Constant(std::string value) : value_(std::move(value))
{
}

EvalType Constant::eval() const
{
    // We can put this unsigned long long or double into a signed long long
    // The bit representation will be the same
    if (value_.find_first_of("'\"") != std::string::npos)
    {
        return static_cast<uint64_t>(getChar());
    }
    if (value_.find_first_of("eEpP.") != std::string::npos)
    {
        // std::stod does not work for Inf, NaN or subnormals
        return std::strtod(value_.c_str(), nullptr);
    }
    else if (std::stoul(value_) > std::numeric_limits<int64_t>::max())
    {
        return std::stoul(value_);
    }
    return std::stol(value_);
}

char Constant::getChar() const
{
    if (value_.size() == 3)
    {
        return value_[1];
    }
    if (value_[1] == '\\')
    {
        return escapeChars.at(value_[2]);
    }

    return 0;
}

const std::unordered_map<char, char> Constant::escapeChars = {
    {'a', '\a'},
    {'b', '\b'},
    {'f', '\f'},
    {'n', '\n'},
    {'r', '\r'},
    {'t', '\t'},
    {'v', '\v'},
    {'\\', '\\'},
    {'\'', '\''},
    {'\"', '\"'},
    {'?', '\?'},
    {'0', '\0'}};

EvalType UnaryOp::eval() const
{
    auto e = expr_->eval();

    // Helper function to run the operation
    auto runOp = [this](auto expr) -> EvalType
    {
        switch (op_)
        {
        case Op::PLUS:
            return expr;
        case Op::MINUS:
            return -expr;
        case Op::NOT:
            if constexpr (std::is_integral_v<decltype(expr)>)
            {
                return ~expr;
            }
            else
            {
                throw std::runtime_error(
                    "Bitwise NOT only allowed on integers");
            }
        case Op::LNOT:
            return static_cast<uint64_t>(!expr);
        case Op::POST_DEC:
            return expr--;
        case Op::POST_INC:
            return expr++;
        case Op::PRE_DEC:
            return --expr;
        case Op::PRE_INC:
            return ++expr;
        case Op::ADDR:
        case Op::DEREF:
            throw std::runtime_error(
                "Address and dereference not allowed here");
        }
    };

    if (e.is<double>())
    {
        return runOp(*e.getDouble());
    }
    else if (e.is<uint64_t>())
    {
        return runOp(*e.getUInt());
    }
    else if (e.is<int64_t>())
    {
        return runOp(*e.getInt());
    }

    return {};
}

StringLiteral::StringLiteral(std::string value) : originalValue_(value)
{
    value_.reserve(value.size());

    for (size_t i = 1; i < value.size() - 1; i++)
    {
        if (value[i] == '\\')
        {
            i++;
            value_ += Constant::escapeChars.at(value[i]);
        }
        else
        {
            value_ += value[i];
        }
    }
}

EvalType StringLiteral::eval() const
{
    return value_;
}

} // namespace AST