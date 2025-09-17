#include <ostream>

#include "util/source_location/SourceLocation.hpp"

std::ostream&
operator<<(
    std::ostream& os,
    const util::SourceLocation& sourceLocation
) {
    os << sourceLocation.file_name() << " ";
    os << "(line " << sourceLocation.line() << ") - ";
    os << sourceLocation.function_name();
    return os; 
}

