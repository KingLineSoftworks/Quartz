#pragma once

#include "util/macros.hpp"

/**
 * @brief This file is seperated from quartz/Core.hpp because the Quartz source doesn't
 * need to know about the application information
 */

/**
 * @brief This is so we don't get linting warnings showing that the version macros
 * are undefined. By doing this the macros will always be defined
 */
#ifdef APPLICATION_VERSION

#else
#define APPLICATION_MAJOR_VERSION -1
#define APPLICATION_MINOR_VERSION -1
#define APPLICATION_PATCH_VERSION -1
#endif

/**
 * @brief A macro allowing us to assert at compile time that we have information about Quartz's
 * version from preprocessor directives. This is essentially just a check to ensure that we were
 * compiled correctly with all of the information that we need.
 */
#ifdef DEBUG
#define ASSERT_APPLICATION_VERSION() \
static_assert(APPLICATION_MAJOR_VERSION >= 0); \
static_assert(APPLICATION_MINOR_VERSION >= 0); \
static_assert(APPLICATION_PATCH_VERSION >= 0); \
REQUIRE_SEMICOLON
#else
#define ASSERT_APPLICATION_VERSION() \
REQUIRE_SEMICOLON
#endif