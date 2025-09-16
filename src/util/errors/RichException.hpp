#pragma once

#include <ostream>
#include <sstream>
#include <string>

#include "util/platform.hpp"

#ifdef ON_MAC

#include "util/source_location/SourceLocation.hpp"

namespace std {
    using source_location = util::SourceLocation;
    using stacktrace = util::SourceLocation;
}

#else

#include <source_location>
#include <stacktrace>

#endif

std::ostream& operator<<(std::ostream& os, const std::source_location& sourceLocation);

namespace util {
    template <typename Data_t>
    class RichException;

    // Some general aliases
    using IntException = RichException<int32_t>;
    using UintException = RichException<uint32_t>;
    using FloatException = RichException<float>;
    using DoubleException = RichException<double>;
    using StringException = RichException<std::string>;
    using PointerException = RichException<void*>;
}

template<typename Data_t>
class util::RichException {
public:
    RichException(
        const std::string& message,
        const Data_t& data,
        const std::source_location& sourceLocation,
        const std::stacktrace& stacktrace
    ) :
        m_message(message),
        m_data(data),
        m_sourceLocation(sourceLocation),
        m_stacktrace(stacktrace)
    {}

    const std::string& what() const { return m_message; }
    const Data_t& data() const { return m_data; }
    const std::source_location& where() const { return m_sourceLocation; }
    const std::stacktrace& trace() const { return m_stacktrace; }

    const std::string toString() const {
        std::ostringstream os;
        os << *this;
        return os.str();
    }

private:
    std::string m_message;
    Data_t m_data;
    const std::source_location m_sourceLocation;
    const std::stacktrace m_stacktrace;
};

template<typename Data_t>
std::ostream&
operator<<(
    std::ostream& os,
    const util::RichException<Data_t>& e
) {
    os << "What:\n  " << e.what() << "\n";
    os << "Where:\n  " << e.where() << "\n";
    os << "Trace:\n" << e.trace() << "\n";
    return os;
}

