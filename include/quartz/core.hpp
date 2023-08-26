#pragma once

#include "util/macros.hpp"

/**
 * @brief This is so we don't get linting warnings showing that the version macros
 * are undefined. By doing this the macros will always be defined
 */
#if defined QUARTZ_VERSION

#else
#define QUARTZ_MAJOR_VERSION -1
#define QUARTZ_MINOR_VERSION -1
#define QUARTZ_PATCH_VERSION -1
#endif

/**
 * @brief A macro allowing us to assert at compile time that we have information about Gemstone's
 * version from preprocessor directives. This is essentially just a check to ensure that we were
 * compiled correctly with all of the information that we need.
 */
#if defined DEBUG
#define ASSERT_QUARTZ_VERSION() \
static_assert(QUARTZ_MAJOR_VERSION >= 0); \
static_assert(QUARTZ_MINOR_VERSION >= 0); \
static_assert(QUARTZ_PATCH_VERSION >= 0); \
REQUIRE_SEMICOLON
#else
#define ASSERT_QUARTZ_VERSION() \
REQUIRE_SEMICOLON
#endif
