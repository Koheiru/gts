#pragma once

#include <cstddef>
#include <iterator>
#include <memory>

// ----------------------------------------------------------------------------
namespace gts {

// ----------------------------------------------------------------------------
template <class error_code>
class basic_error
{
public:
    explicit basic_error(basic_error<error_code>* nested = nullptr)
        : m_code(error_code::no_error), m_remains(), m_nested(nested)
    {}

    explicit basic_error(error_code code, basic_error<error_code>* nested = nullptr)
        : m_code(code), m_remains(), m_nested(nested)
    {}

    basic_error(error_code code, std::ptrdiff_t remains, basic_error<error_code>* nested = nullptr)
        : m_code(code), m_remains(remains), m_nested(nested)
    {}

    error_code code() const
    {
        return m_code;
    }

    std::ptrdiff_t remains() const
    {
        return m_remains;
    }

    operator bool() const
    {
        return (m_code != error_code::no_error);
    }

    const basic_error* nested() const
    {
        return m_nested.get();
    }

private:
    error_code      m_code;
    std::ptrdiff_t  m_remains;
    std::unique_ptr<basic_error<error_code>> m_nested;
};

// ----------------------------------------------------------------------------
template <class Iterator, class error_code>
Iterator create_error(Iterator end, error_code code, basic_error<error_code>* error)
{
    if (error != nullptr)
    {
        std::unique_ptr<basic_error<error_code>> nested(new basic_error<error_code>());
        *nested = std::move(*error);
        *error = basic_error<error_code>(code, nested.release());
    }
    return end;
}

// ----------------------------------------------------------------------------
template <class Iterator, class error_code>
Iterator create_error(Iterator pos, Iterator end, error_code code, basic_error<error_code>* error)
{
    if (error != nullptr)
    {
        std::unique_ptr<basic_error<error_code>> nested(new basic_error<error_code>());
        *nested = std::move(*error);
        *error = basic_error<error_code>(code, std::distance(pos, end), nested.release());
    }
    return end;
}

} // namespace gts