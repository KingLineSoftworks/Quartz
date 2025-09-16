#include <ostream>

#include "util/errors/RichException.hpp"

std::ostream&
operator<<(
    std::ostream& os,
    const std::source_location& sourceLocation
) {
    os << sourceLocation.file_name() << " ";
    os << "(line " << sourceLocation.line() << ") - ";
    os << sourceLocation.function_name();
    return os; 
}

