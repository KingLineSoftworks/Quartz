#====================================================================
# The errors and exceptions utility library
#====================================================================
add_library(
    UTIL_Errors
    SHARED
    RichException.hpp
    RichException.cpp
)

target_include_directories(
    UTIL_Errors
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)

target_compile_options(
    UTIL_Errors
    PUBLIC
    ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    UTIL_Errors
    PUBLIC
    ${QUARTZ_COMPILE_DEFINITIONS}
)

# On mac we do not have access to std::stacktrace and std::source_location,
# so we are going to provide our own versions instead
set(STACKTRACE_LIBS "")
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND STACKTRACE_LIBS UTIL_SourceLocation)
else ()
    list(APPEND STACKTRACE_LIBS stdc++exp)
endif ()

target_link_libraries(
    UTIL_Errors

    PUBLIC
    ${STACKTRACE_LIBS}
)
