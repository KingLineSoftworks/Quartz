#pragma once

#include <iostream>

/**
 * @brief Make sure all of the necessary preprocessor definitions are present if we are in debug mode
 */

#if defined QUARTZ_DEBUG

#if !defined DEBUG
#define DEBUG
#endif

#if !defined _DEBUG
#define _DEBUG
#endif

#endif

/**
 * @brief If we are in test mode we still want to have debug symbols (handled with -g flag) for under the hood but we want
 * our stuff to see we are in test mode and not debug mode
 */

#if defined QUARTZ_TEST

// Undefine the first standard (?) debug macro
#if defined DEBUG
#undef DEBUG
#endif

// Undefine the second standard (?) debug macro
#if defined _DEBUG
#undef _DEBUG
#endif

// Undefine the quartz debug macro
#if defined QUARTZ_DEBUG
#undef QUARTZ_DEBUG
#endif

#endif

/**
 * @brief If we are in release mode we want to ensure that all of the debugging and testing macros
 * are not defined but the "not debug" and release macros are defined
 */

#if defined QUARTZ_RELEASE

// Undefine the first standard (?) debug macro
#if defined DEBUG
#undef DEBUG
#endif

// Undefine the second standard (?) debug macro
#if defined _DEBUG
#undef _DEBUG
#endif

// Undefine the quartz debug macro
#if defined QUARTZ_DEBUG
#undef QUARTZ_DEBUG
#endif

// Undefine the quartz test macro
#if defined QUARTZ_TEST
#undef QUARTZ_TEST
#endif

// Define the not debug macro
#if !defined NDEBUG
#define NDEBUG
#endif

// Define the  release macro
#if !defined RELEASE
#define RELEASE
#endif

#endif

/**
 * @brief A simple macro to be placed on the end of a macro serving as a function.
 * This requires the macros ending with this to have a semicolon following it.
 * 
 * @example Check out ASSERT_QUARTZ_VERSION() in quartz/core.hpp
 */
#define REQUIRE_SEMICOLON static_assert(true)

/**
 * @brief A macro to use to get rid of the unused variable/parameter warnings and
 * errors when prototyping a function. Use this shit when you're declaring a variable
 * or a parameter or a member.
 */
#define UNUSED [[maybe_unused]]

/**
 * @brief A macro to generate a unique variable name given the base name for that
 * variable.
 * 
 * @example See quartz/util/logger/Logger.hpp LOG_SCOPE_CHANGE()
 */
#define CONCAT(a, b) CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a ## b
#define UNIQUE_NAME(baseName) CONCAT(baseName, __LINE__)

/**
 * @brief A macro allowing us to run time assert and output a message if we fail
 *
 * @todo 2024/06/20 Make this work with our logging system
 */
#if defined QUARTZ_DEBUG
#define QUARTZ_ASSERT(condition, message)     \
    do {                                      \
        if (! (condition)) {                  \
            std::cerr << "Assertion (" #condition ") failed in " << __FILE__ << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate();                 \
        }                                     \
    } while (false)
#else
#define QUARTZ_ASSERT(condition, message) static_assert(true)
#endif

/**
 * @brief Macros determining the quanitity of things we can have in a scene. These
 * should be defined at compile time by cmake. If these aren't defined for us at
 * compile time then we should set these to values that should invoke errors
 */

#ifndef QUARTZ_MAX_NUMBER_TEXTURES
#define QUARTZ_MAX_NUMBER_TEXTURES -1
#endif

#ifndef QUARTZ_MAX_NUMBER_MATERIALS
#define QUARTZ_MAX_NUMBER_MATERIALS -1
#endif

#ifndef QUARTZ_MAX_NUMBER_POINT_LIGHTS
#define QUARTZ_MAX_NUMBER_POINT_LIGHTS -1
#endif

#ifndef QUARTZ_MAX_NUMBER_SPOT_LIGHTS
#define QUARTZ_MAX_NUMBER_SPOT_LIGHTS -1
#endif
