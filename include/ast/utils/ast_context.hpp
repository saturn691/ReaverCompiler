#pragma once

#include <string>

#include <ty/ty.hpp>

namespace ast
{
    enum class Types;
    using Types_t = ty::Types;

    /**
     * Contextual information to be passed down when recursively searching through
     * the nodes.
     */
    class Context
    {
    public:
        Context() = default;
        ~Context() = default;

        Types_t get_type(const std::string &id) const;

        int bb;
    };
}
