#pragma once

#include <cstdint>
#include <ostream>

namespace util {
    class SourceLocation;
}

/**
 * @brief A simple implementation of std::source_location so we can still compile
 *    on mac because mac for some reason cannot use c++23 features
 *
 *    This is used in RichException.hpp, where we alias std::source_location to
 *    this class. They have the same interface.
 */
class util::SourceLocation {
public:
    static inline constexpr SourceLocation current() noexcept {
        return util::SourceLocation();
    }

    constexpr SourceLocation() noexcept :
        m_line(0),
        m_column(0),
        m_fileName(""),
        m_functionName("")
    {}

    constexpr SourceLocation(
        const uint32_t line,
        const uint32_t column,
        const char* fileName,
        const char* functionName
    ) noexcept :
        m_line(line),
        m_column(column),
        m_fileName(fileName),
        m_functionName(functionName)
    {}

    constexpr uint32_t line() const noexcept { return m_line; };
    constexpr uint32_t column() const noexcept { return m_column; };
    constexpr const char* file_name() const noexcept { return m_fileName; };
    constexpr const char* function_name() const noexcept { return m_functionName; };

private:
    uint32_t m_line;
    uint32_t m_column;
    const char* m_fileName;
    const char* m_functionName;
};

std::ostream& operator<<(std::ostream& os, const util::SourceLocation& sourceLocation);
