#include <stack>

/**
 * Helper class for passing information down the AST.
 */
template <typename T>
class ScopeGuard
{
public:
    ScopeGuard(T &ref, const T &newValue) : ref_(ref), oldValue_(ref)
    {
        ref = newValue;
    }
    ~ScopeGuard()
    {
        ref_ = oldValue_;
    }

private:
    T &ref_;
    T oldValue_;
};

template <typename U>
class ScopeGuard<std::stack<U>>
{
public:
    ScopeGuard(std::stack<U> &ref, U newValue) : ref_(ref)
    {
        ref.push(std::move(newValue));
    }
    ~ScopeGuard()
    {
        ref_.pop();
    }

private:
    std::stack<U> &ref_;
};