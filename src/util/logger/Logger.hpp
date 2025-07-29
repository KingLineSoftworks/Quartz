#pragma once

#include <array>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "spdlog/async.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"

#include "util/macros.hpp"
#include "util/errors/AssetErrors.hpp"
#include "util/errors/VulkanErrors.hpp"

namespace util {
    class Logger;
    class LoggerUnitTestClient;

#if defined QUARTZ_DEBUG
    using spdlog_logger_t = spdlog::logger;
#else
    using spdlog_logger_t = spdlog::async_logger;
#endif
}

/**
 * @brief A singleton-esque logger wrapping a spdlog async logger. This exposes a map
 * of loggers, each of which has their own logging level associated with it. To use it
 * be sure to register a logger with the registerLogger function.
 */
class util::Logger {
public: // public classes and enums
    /**
     * @brief An enum class representing each of the different logging levels
     */
    enum class Level {
        trace = 0,
        debug = 1,
        info = 2,
        warning = 3,
        error = 4,
        critical = 5,
        off = 6
    };

    /**
     * @brief A simple struct used for registering loggers. This pairs a logger's
     * name with its default logging level. This is essentially just a pair with
     * named elements
     */
    struct RegistrationInfo {
        const char* loggerName;
        const util::Logger::Level level;
    };

    /**
     * @brief A class to manage the "scope" (indentation) of the logging statements. Constructing an
     * instance of one of these will log an opening curly brace and increment the indentation count
     * for all logging statements until the Scoper instance falls out of scope, where then the
     * indendation count is decremented and a closing curly brace is logged
     */
    class Scoper {
    public: // public static functions
        static uint32_t getIndentationCount() { return util::Logger::Scoper::indentationCount; }

    public: // public member functions
        Scoper(const std::string& loggerName, const util::Logger::Level level);
        ~Scoper();

        Scoper(const Scoper& other) = delete;
        Scoper& operator=(const Scoper& other) = delete;

        Scoper(Scoper&& other) = delete;
        Scoper& operator=(Scoper&& other) = delete;

    private: // private static variables
        static uint32_t indentationCount;

    private: // private member variables
        const std::string m_loggerName;
        const util::Logger::Level m_level;
    };

public: // public static functions
    static void setShouldLogPreamble(const bool _shouldLogPreamble) { util::Logger::shouldLogPreamble = _shouldLogPreamble; }

    static void registerLogger(const std::string& loggerName, const util::Logger::Level level);

    template<size_t N>
    static void registerLoggers(const std::array<const util::Logger::RegistrationInfo, N>& loggerInfos) {
        for (const util::Logger::RegistrationInfo& loggerInfo : loggerInfos) {
            util::Logger::registerLogger(loggerInfo.loggerName, loggerInfo.level);
        }
    }

    static void setLevel(const std::string& loggerName, const util::Logger::Level desiredLevel);
    static void setLevels(const std::vector<util::Logger::RegistrationInfo>& loggerInfos);

    /**
     * @brief Functions to actually log messages
     * 
     * @tparam Args The variadic arguments to log
     * @param loggerName The name of the logger we should be using to log
     * @param format The formatting string
     * @param args The variadic arguments to apply to the formatting string
     */

    template<typename... Args>
    static void trace(UNUSED const std::string& loggerName, UNUSED const std::string& format, UNUSED Args&&... args) {
        #if defined(QUARTZ_DEBUG) || defined(QUARTZ_TEST)
        assertInitialized();
        util::Logger::loggerPtrMap[loggerName]->trace(fmt::runtime(util::Logger::createIndentationString() + format), args...);
        #endif
    }

    template<typename... Args>
    static void debug(UNUSED const std::string& loggerName, UNUSED const std::string& format, UNUSED Args&&... args) {
        #if defined(QUARTZ_DEBUG) || defined(QUARTZ_TEST)
        assertInitialized();
        util::Logger::loggerPtrMap[loggerName]->debug(fmt::runtime(util::Logger::createIndentationString() + format), args...);
        #endif
    }

    template<typename... Args>
    static void info(UNUSED const std::string& loggerName, UNUSED const std::string& format, UNUSED Args&&... args) {
        #if defined(QUARTZ_DEBUG) || defined(QUARTZ_TEST)
        assertInitialized();
        util::Logger::loggerPtrMap[loggerName]->info(fmt::runtime(util::Logger::createIndentationString() + format), args...);
        #endif
    }

    template<typename... Args>
    static void warning(const std::string& loggerName, const std::string& format, Args&&... args) {
        assertInitialized();
        util::Logger::loggerPtrMap[loggerName]->warn(fmt::runtime(util::Logger::createIndentationString() + format), args...);
    }

    template<typename... Args>
    static void error(const std::string& loggerName, const std::string& format, Args&&... args) {
        assertInitialized();
        util::Logger::loggerPtrMap[loggerName]->error(fmt::runtime(util::Logger::createIndentationString() + format), args...);
    }

    template<typename... Args>
    static void critical(const std::string& loggerName, const std::string& format, Args&&... args) {
        assertInitialized();
        util::Logger::loggerPtrMap[loggerName]->critical(fmt::runtime(util::Logger::createIndentationString() + format), args...);
    }

    /**
     * @brief Log a message with the correct logger at the correct level.
     * 
     * @tparam Args The types of the variadic arguments we are using
     * @param loggerName The name of the logger to use
     * @param level The logging level to log the message at
     * @param format The formatting string
     * @param args All of the variadic arguments to log
     */
    template<typename... Args>
    static void log(const std::string& loggerName, const util::Logger::Level level, const std::string& format, Args&&... args) {
        switch (level) {
            case util::Logger::Level::trace:
                #if defined(QUARTZ_DEBUG) || defined(QUARTZ_TEST)
                util::Logger::trace(loggerName, format, args...);
                #endif
                break;
            case util::Logger::Level::debug:
                #if defined(QUARTZ_DEBUG) || defined(QUARTZ_TEST)
                util::Logger::debug(loggerName, format, args...);
                #endif
                break;
            case util::Logger::Level::info:
                #if defined(QUARTZ_DEBUG) || defined(QUARTZ_TEST)
                util::Logger::info(loggerName, format, args...);
                #endif
                break;
            case util::Logger::Level::warning:
                util::Logger::warning(loggerName, format, args...);
                break;
            case util::Logger::Level::error:
                util::Logger::error(loggerName, format, args...);
                break;
            case util::Logger::Level::critical:
                util::Logger::critical(loggerName, format, args...);
                break;
            case util::Logger::Level::off:
                break;
        }
    }

public: // public member functions
    Logger() = delete;

private: // private static functions
    static void init();
    static void assertInitialized();

    static util::Logger::Level getLevel(const std::string& loggerName);

    static std::string createIndentationString();

private: // private static variables
    static bool initialized;
    static bool shouldLogPreamble;

    static std::map<std::string, util::Logger::Level> loggerNameDefaultLevelMap;
    static std::map<std::string, util::Logger::Level> loggerNameLevelMap;

    static std::shared_ptr<spdlog::details::thread_pool> threadPool;
    static std::vector<spdlog::sink_ptr> sinkPtrs;
    static std::map<std::string, std::shared_ptr<util::spdlog_logger_t>> loggerPtrMap;

private: // friends
    friend LoggerUnitTestClient;
};

/**
 * @brief Helper functions
 */

std::ostream& operator<<(std::ostream& os, const util::Logger::Level level);

/**
 * @brief Create a logger
 */

#define DECLARE_LOGGER(name, level) \
    namespace quartz {              \
    namespace loggers {             \
        constexpr util::Logger::RegistrationInfo name = {#name, util::Logger::Level::level}; \
    }                               \
    }                               \
    REQUIRE_SEMICOLON

/**
 * @brief Create a group of loggers and a macro to easily register them
 */

#define DECLARE_LOGGER_GROUP(groupName, groupSize, ...) \
    namespace quartz {                                  \
    namespace loggers {                                 \
        constexpr std::array<const util::Logger::RegistrationInfo, groupSize> groupName##_LOGGER_INFOS = { __VA_ARGS__ }; \
    }                                                   \
    }                                                   \
    REQUIRE_SEMICOLON

/**
 * @brief A macro to easily register your group of loggers
 */

#define REGISTER_LOGGER_GROUP(groupName) \
    util::Logger::registerLoggers(quartz::loggers::groupName##_LOGGER_INFOS)

/**
 * @brief A macro to easily set all of the levels of all loggers in a group whilest registering that group
 */
#define REGISTER_LOGGER_GROUP_WITH_LEVEL(groupName, level)                                                      \
    util::Logger::registerLoggers(quartz::loggers::groupName##_LOGGER_INFOS);                                   \
    for (const util::Logger::RegistrationInfo& registrationInfo : quartz::loggers::groupName##_LOGGER_INFOS) {    \
         util::Logger::setLevel(registrationInfo.loggerName, util::Logger::Level::level);                       \
    }                                                                                                           \
    REQUIRE_SEMICOLON

/**
 * @brief Dictate which logger you are going to be using
 * 
 * This defines a function to get a reference to the necessary logger registration information in the quartz::logger namespace=
 */

#define USE_LOGGER(loggerRegistrationInfoName) \
    const util::Logger::RegistrationInfo& getLoggerRegistrationInfo() const { return quartz::loggers::loggerRegistrationInfoName; } \
    REQUIRE_SEMICOLON

/**
 * @brief Log something
 * 
 * @note the first argument of the variadic arguments must be a formatting string
 *
 * @todo Make the compiled away LOG_*this functions do a compile time check for if this->getLoggerRegistrationInfo() exists
 */

#if defined(QUARTZ_DEBUG) || defined(QUARTZ_TEST)
#define LOG_TRACE(REGISTRATION_NAME, ...) \
    util::Logger::trace(quartz::loggers::REGISTRATION_NAME.loggerName, __VA_ARGS__)
#define LOG_TRACEthis(...) \
    util::Logger::trace(this->getLoggerRegistrationInfo().loggerName, __VA_ARGS__)

#define LOG_DEBUG(REGISTRATION_NAME, ...) \
    util::Logger::debug(quartz::loggers::REGISTRATION_NAME.loggerName, __VA_ARGS__)
#define LOG_DEBUGthis(...) \
    util::Logger::debug(this->getLoggerRegistrationInfo().loggerName, __VA_ARGS__)

#define LOG_INFO(REGISTRATION_NAME, ...) \
    util::Logger::info(quartz::loggers::REGISTRATION_NAME.loggerName, __VA_ARGS__)
#define LOG_INFOthis(...) \
    util::Logger::info(this->getLoggerRegistrationInfo().loggerName, __VA_ARGS__)
#else
#define LOG_TRACE(REGISTRATION_NAME, ...) \
    REQUIRE_SEMICOLON
#define LOG_TRACEthis(...) \
    REQUIRE_SEMICOLON

#define LOG_DEBUG(REGISTRATION_NAME, ...) \
    REQUIRE_SEMICOLON
#define LOG_DEBUGthis(...) \
    REQUIRE_SEMICOLON

#define LOG_INFO(REGISTRATION_NAME, ...) \
    REQUIRE_SEMICOLON
#define LOG_INFOthis(...) \
    REQUIRE_SEMICOLON
#endif

#define LOG_WARNING(REGISTRATION_NAME, ...) \
    util::Logger::warning(quartz::loggers::REGISTRATION_NAME.loggerName, __VA_ARGS__)
#define LOG_WARNINGthis(...) \
    util::Logger::warning(this->getLoggerRegistrationInfo().loggerName, __VA_ARGS__)

#define LOG_ERROR(REGISTRATION_NAME, ...) \
    util::Logger::error(quartz::loggers::REGISTRATION_NAME.loggerName, __VA_ARGS__)
#define LOG_ERRORthis(...) \
    util::Logger::error(this->getLoggerRegistrationInfo().loggerName, __VA_ARGS__)

#define LOG_CRITICAL(REGISTRATION_NAME, ...) \
    util::Logger::critical(quartz::loggers::REGISTRATION_NAME.loggerName, __VA_ARGS__)
#define LOG_CRITICALthis(...) \
    util::Logger::critical(this->getLoggerRegistrationInfo().loggerName, __VA_ARGS__)

#define LOG_THROW(REGISTRATION_NAME, ERROR_TYPE, ...) \
    util::Logger::critical(quartz::loggers::REGISTRATION_NAME.loggerName, __VA_ARGS__); \
    throw ERROR_TYPE(fmt::format(__VA_ARGS__))
#define LOG_THROWthis(ERROR_TYPE, ...) \
    util::Logger::critical(this->getLoggerRegistrationInfo().loggerName, __VA_ARGS__); \
    throw ERROR_TYPE(fmt::format(__VA_ARGS__))

/**
 * @brief Log a scope change
 */

#if defined(QUARTZ_DEBUG) || defined(QUARTZ_TEST)
#define LOG_SCOPE_CHANGE_TRACE(REGISTRATION_NAME) \
    const util::Logger::Scoper UNIQUE_NAME(scoper)(quartz::loggers::REGISTRATION_NAME.loggerName, util::Logger::Level::trace)
#define LOG_SCOPE_CHANGE_TRACEthis() \
    const util::Logger::Scoper UNIQUE_NAME(scoper)(this->getLoggerRegistrationInfo().loggerName, util::Logger::Level::trace)

#define LOG_SCOPE_CHANGE_DEBUG(REGISTRATION_NAME) \
    const util::Logger::Scoper UNIQUE_NAME(scoper)(quartz::loggers::REGISTRATION_NAME.loggerName, util::Logger::Level::debug)
#define LOG_SCOPE_CHANGE_DEBUGthis() \
    const util::Logger::Scoper UNIQUE_NAME(scoper)(this->getLoggerRegistrationInfo().loggerName, util::Logger::Level::debug)

#define LOG_SCOPE_CHANGE_INFO(REGISTRATION_NAME) \
    const util::Logger::Scoper UNIQUE_NAME(scoper)(quartz::loggers::REGISTRATION_NAME.loggerName, util::Logger::Level::info)
#define LOG_SCOPE_CHANGE_INFOthis() \
    const util::Logger::Scoper UNIQUE_NAME(scoper)(this->getLoggerRegistrationInfo().loggerName, util::Logger::Level::info)
#else
#define LOG_SCOPE_CHANGE_TRACE(REGISTRATION_NAME) \
    REQUIRE_SEMICOLON
#define LOG_SCOPE_CHANGE_TRACEthis() \
    REQUIRE_SEMICOLON

#define LOG_SCOPE_CHANGE_DEBUG(REGISTRATION_NAME) \
    REQUIRE_SEMICOLON
#define LOG_SCOPE_CHANGE_DEBUGthis() \
    REQUIRE_SEMICOLON

#define LOG_SCOPE_CHANGE_INFO(REGISTRATION_NAME) \
    REQUIRE_SEMICOLON
#define LOG_SCOPE_CHANGE_INFOthis() \
    REQUIRE_SEMICOLON
#endif

#define LOG_SCOPE_CHANGE_WARNING(REGISTRATION_NAME) \
    const util::Logger::Scoper UNIQUE_NAME(scoper)(quartz::loggers::REGISTRATION_NAME.loggerName, util::Logger::Level::warning)
#define LOG_SCOPE_CHANGE_WARNINGthis() \
    const util::Logger::Scoper UNIQUE_NAME(scoper)(this->getLoggerRegistrationInfo().loggerName, util::Logger::Level::warning)

#define LOG_SCOPE_CHANGE_ERROR(REGISTRATION_NAME) \
    const util::Logger::Scoper UNIQUE_NAME(scoper)(quartz::loggers::REGISTRATION_NAME.loggerName, util::Logger::Level::error)
#define LOG_SCOPE_CHANGE_ERRORthis() \
    const util::Logger::Scoper UNIQUE_NAME(scoper)(this->getLoggerRegistrationInfo().loggerName, util::Logger::Level::error)

#define LOG_SCOPE_CHANGE_CRITICAL(REGISTRATION_NAME) \
    const util::Logger::Scoper UNIQUE_NAME(scoper)(quartz::loggers::REGISTRATION_NAME.loggerName, util::Logger::Level::critical)
#define LOG_SCOPE_CHANGE_CRITICALthis() \
    const util::Logger::Scoper UNIQUE_NAME(scoper)(this->getLoggerRegistrationInfo().loggerName, util::Logger::Level::critical)

/**
 * @brief log a function call *WITHOUT* scope change
 * 
 * @details The stuff inside of the __VA_OPT__ macro is only given if __VA_ARGS__ is not empty.
 * So if we only provide a format string and no additional arguments, the __VA_OPT__(,) will not be supplied.
 * @details We check if the format string is empty to determine if we need an extra space before our final bracket,
 * so we can only output 1 space instead of 2 if there is nothing inside of them
 */

#if defined(QUARTZ_DEBUG) || defined(QUARTZ_TEST)
#define LOG_FUNCTION_CALL_TRACE(REGISTRATION_NAME, format, ...) \
    util::Logger::trace( \
        quartz::loggers::REGISTRATION_NAME.loggerName, \
        __PRETTY_FUNCTION__ + std::string(" [ ") + format + (std::string(format) == std::string("") ? "" : " ") + std::string("]") __VA_OPT__(,) \
        __VA_ARGS__ \
    )
#define LOG_FUNCTION_CALL_TRACEthis(format, ...)  \
    util::Logger::trace( \
        this->getLoggerRegistrationInfo().loggerName, \
        __PRETTY_FUNCTION__ + std::string(" [ ") + format + (std::string(format) == std::string("") ? "" : " ") + std::string("]") __VA_OPT__(,) \
        __VA_ARGS__ \
    )

#define LOG_FUNCTION_CALL_DEBUG(REGISTRATION_NAME, format, ...) \
    util::Logger::debug( \
        quartz::loggers::REGISTRATION_NAME.loggerName, \
        __PRETTY_FUNCTION__ + std::string(" [ ") + format + (std::string(format) == std::string("") ? "" : " ") + std::string("]") __VA_OPT__(,) \
        __VA_ARGS__ \
    )
#define LOG_FUNCTION_CALL_DEBUGthis(format, ...)  \
    util::Logger::debug( \
        this->getLoggerRegistrationInfo().loggerName, \
        __PRETTY_FUNCTION__ + std::string(" [ ") + format + (std::string(format) == std::string("") ? "" : " ") + std::string("]") __VA_OPT__(,) \
        __VA_ARGS__ \
    )

#define LOG_FUNCTION_CALL_INFO(REGISTRATION_NAME, format, ...) \
    util::Logger::info( \
        quartz::loggers::REGISTRATION_NAME.loggerName, \
        __PRETTY_FUNCTION__ + std::string(" [ ") + format + (std::string(format) == std::string("") ? "" : " ") + std::string("]") __VA_OPT__(,) \
        __VA_ARGS__ \
    )
#define LOG_FUNCTION_CALL_INFOthis(format, ...)  \
    util::Logger::info( \
        this->getLoggerRegistrationInfo().loggerName, \
        __PRETTY_FUNCTION__ + std::string(" [ ") + format + (std::string(format) == std::string("") ? "" : " ") + std::string("]") __VA_OPT__(,) \
        __VA_ARGS__ \
    )
#else
#define LOG_FUNCTION_CALL_TRACE(REGISTRATION_NAME, format, ...) \
    REQUIRE_SEMICOLON
#define LOG_FUNCTION_CALL_TRACEthis(format, ...)  \
    REQUIRE_SEMICOLON

#define LOG_FUNCTION_CALL_DEBUG(REGISTRATION_NAME, format, ...) \
    REQUIRE_SEMICOLON
#define LOG_FUNCTION_CALL_DEBUGthis(format, ...)  \
    REQUIRE_SEMICOLON

#define LOG_FUNCTION_CALL_INFO(REGISTRATION_NAME, format, ...) \
    REQUIRE_SEMICOLON
#define LOG_FUNCTION_CALL_INFOthis(format, ...)  \
    REQUIRE_SEMICOLON
#endif

#define LOG_FUNCTION_CALL_WARNING(REGISTRATION_NAME, format, ...) \
    util::Logger::warning( \
        quartz::loggers::REGISTRATION_NAME.loggerName, \
        __PRETTY_FUNCTION__ + std::string(" [ ") + format + (std::string(format) == std::string("") ? "" : " ") + std::string("]") __VA_OPT__(,) \
        __VA_ARGS__ \
    )
#define LOG_FUNCTION_CALL_WARNINGthis(format, ...)  \
    util::Logger::warning( \
        this->getLoggerRegistrationInfo().loggerName, \
        __PRETTY_FUNCTION__ + std::string(" [ ") + format + (std::string(format) == std::string("") ? "" : " ") + std::string("]") __VA_OPT__(,) \
        __VA_ARGS__ \
    )

#define LOG_FUNCTION_CALL_ERROR(REGISTRATION_NAME, format, ...) \
    util::Logger::error( \
        quartz::loggers::REGISTRATION_NAME.loggerName, \
        __PRETTY_FUNCTION__ + std::string(" [ ") + format + (std::string(format) == std::string("") ? "" : " ") + std::string("]") __VA_OPT__(,) \
        __VA_ARGS__ \
    )
#define LOG_FUNCTION_CALL_ERRORthis(format, ...)  \
    util::Logger::error( \
        this->getLoggerRegistrationInfo().loggerName, \
        __PRETTY_FUNCTION__ + std::string(" [ ") + format + (std::string(format) == std::string("") ? "" : " ") + std::string("]") __VA_OPT__(,) \
        __VA_ARGS__ \
    )

#define LOG_FUNCTION_CALL_CRITICAL(REGISTRATION_NAME, format, ...) \
    util::Logger::critical( \
        quartz::loggers::REGISTRATION_NAME.loggerName, \
        __PRETTY_FUNCTION__ + std::string(" [ ") + format + (std::string(format) == std::string("") ? "" : " ") + std::string("]") __VA_OPT__(,) \
        __VA_ARGS__ \
    )
#define LOG_FUNCTION_CALL_CRITICALthis(format, ...)  \
    util::Logger::critical( \
        this->getLoggerRegistrationInfo().loggerName, \
        __PRETTY_FUNCTION__ + std::string(" [ ") + format + (std::string(format) == std::string("") ? "" : " ") + std::string("]") __VA_OPT__(,) \
        __VA_ARGS__ \
    )

/**
 * @brief log a function call and its scope change
 */

#if defined(QUARTZ_DEBUG) || defined(QUARTZ_TEST)
#define LOG_FUNCTION_SCOPE_TRACE(REGISTRATION_NAME, format, ...) \
    LOG_FUNCTION_CALL_TRACE(REGISTRATION_NAME, format, __VA_ARGS__); \
    LOG_SCOPE_CHANGE_TRACE(REGISTRATION_NAME)
#define LOG_FUNCTION_SCOPE_TRACEthis(format, ...) \
    LOG_FUNCTION_CALL_TRACEthis(format, __VA_ARGS__); \
    LOG_SCOPE_CHANGE_TRACEthis()
    
#define LOG_FUNCTION_SCOPE_DEBUG(REGISTRATION_NAME, format, ...) \
    LOG_FUNCTION_CALL_DEBUG(REGISTRATION_NAME, format, __VA_ARGS__); \
    LOG_SCOPE_CHANGE_DEBUG(REGISTRATION_NAME)
#define LOG_FUNCTION_SCOPE_DEBUGthis(format, ...) \
    LOG_FUNCTION_CALL_DEBUGthis(format, __VA_ARGS__); \
    LOG_SCOPE_CHANGE_DEBUGthis()
    
#define LOG_FUNCTION_SCOPE_INFO(REGISTRATION_NAME, format, ...) \
    LOG_FUNCTION_CALL_INFO(REGISTRATION_NAME, format, __VA_ARGS__); \
    LOG_SCOPE_CHANGE_INFO(REGISTRATION_NAME)
#define LOG_FUNCTION_SCOPE_INFOthis(format, ...) \
    LOG_FUNCTION_CALL_INFOthis(format, __VA_ARGS__); \
    LOG_SCOPE_CHANGE_INFOthis()
#else
#define LOG_FUNCTION_SCOPE_TRACE(REGISTRATION_NAME, format, ...) \
    REQUIRE_SEMICOLON
#define LOG_FUNCTION_SCOPE_TRACEthis(format, ...) \
    REQUIRE_SEMICOLON

#define LOG_FUNCTION_SCOPE_DEBUG(REGISTRATION_NAME, format, ...) \
    REQUIRE_SEMICOLON
#define LOG_FUNCTION_SCOPE_DEBUGthis(format, ...) \
    REQUIRE_SEMICOLON

#define LOG_FUNCTION_SCOPE_INFO(REGISTRATION_NAME, format, ...) \
    REQUIRE_SEMICOLON
#define LOG_FUNCTION_SCOPE_INFOthis(format, ...) \
    REQUIRE_SEMICOLON
#endif
    
#define LOG_FUNCTION_SCOPE_WARNING(REGISTRATION_NAME, format, ...) \
    LOG_FUNCTION_CALL_WARNING(REGISTRATION_NAME, format, __VA_ARGS__); \
    LOG_SCOPE_CHANGE_WARNING(REGISTRATION_NAME)
#define LOG_FUNCTION_SCOPE_WARNINGthis(format, ...) \
    LOG_FUNCTION_CALL_WARNINGthis(format, __VA_ARGS__); \
    LOG_SCOPE_CHANGE_WARNINGthis()
    
#define LOG_FUNCTION_SCOPE_ERROR(REGISTRATION_NAME, format, ...) \
    LOG_FUNCTION_CALL_ERROR(REGISTRATION_NAME, format, __VA_ARGS__); \
    LOG_SCOPE_CHANGE_ERROR(REGISTRATION_NAME)
#define LOG_FUNCTION_SCOPE_ERRORthis(format, ...) \
    LOG_FUNCTION_CALL_ERRORthis(format, __VA_ARGS__); \
    LOG_SCOPE_CHANGE_ERRORthis()
    
#define LOG_FUNCTION_SCOPE_CRITICAL(REGISTRATION_NAME, format, ...) \
    LOG_FUNCTION_CALL_CRITICAL(REGISTRATION_NAME, format, __VA_ARGS__); \
    LOG_SCOPE_CHANGE_CRITICAL(REGISTRATION_NAME)
#define LOG_FUNCTION_SCOPE_CRITICALthis(format, ...) \
    LOG_FUNCTION_CALL_CRITICALthis(format, __VA_ARGS__); \
    LOG_SCOPE_CHANGE_CRITICALthis()
    
