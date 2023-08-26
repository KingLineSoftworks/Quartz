#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "spdlog/async.h"
#include "spdlog/spdlog.h"

#include "util/macros.hpp"

namespace quartz {
namespace util {
    class Logger;

#if defined QUARTZ_DEBUG
    using spdlog_logger_t = spdlog::logger;
#else
    using spdlog_logger_t = spdlog::async_logger;
#endif
}
}

/**
 * @brief A singleton-esque logger wrapping a spdlog async logger. This exposes a map
 * of loggers, each of which has their own logging level associated with it. To use it
 * be sure to register a logger with the registerLogger function.
 * 
 * @note If you wish to make use of the nice macros at the bottom of this file, be sure
 * to define some LOGGER_NAME string to whatever logger you desire to use. The macros
 * assume that LOGGER_NAME is already defined. If LOGGER_NAME is not defined and you wish
 * to programatically change the logger being used (perhaps in a function taking a logger
 * name parameter) you must use the quartz::util::Logger functions directly
 * 
 * @todo Clamp log levels based on debug vs release mode or something?
 * 
 * @todo ALlow us to set stdout levels vs file levels differently?
 */
class quartz::util::Logger {
public: // public classes and enums
    /**
     * @brief An enum class representing each of the different logging levels
     */
    enum class Level {
        trace,
        debug,
        info,
        warning,
        error,
        critical
    };

    /**
     * @brief A simple struct used for registering loggers. This pairs a logger's
     * name with its desired logging level. This is essentially just a pair with
     * named elements
     */
    struct RegistrationInfo {
        const std::string loggerName;
        const quartz::util::Logger::Level level;
    };

    /**
     * @brief A class to manage the "scope" (indentation) of the logging statements. Constructing an
     * instance of one of these will log an opening curly brace and increment the indentation count
     * for all logging statements until the Scoper instance falls out of scope, where then the
     * indendation count is decremented and a closing curly brace is logged
     */
    class Scoper {
    public: // public static functions
        static uint32_t getIndentationCount() { return quartz::util::Logger::Scoper::indentationCount; }

    public: // public member functions
        Scoper(const std::string& loggerName, const quartz::util::Logger::Level level);
        ~Scoper();

        Scoper(const Scoper& other) = delete;
        Scoper& operator=(const Scoper& other) = delete;

        Scoper(Scoper&& other) = delete;
        Scoper& operator=(Scoper&& other) = delete;

    private: // private static variables
        static uint32_t indentationCount;

    private: // private member variables
        const std::string m_loggerName;
        const quartz::util::Logger::Level m_level;
    };

public: // public static functions
    static void registerLogger(const std::string& loggerName, const quartz::util::Logger::Level level);
    static void registerLoggers(const std::vector<const quartz::util::Logger::RegistrationInfo>& loggerInfos);

    template<typename... Args>
    static void trace(const std::string& loggerName, const std::string& format, Args&&... args) {
        assertInitialized();
        quartz::util::Logger::loggerPtrMap[loggerName]->trace(quartz::util::Logger::createIndentationString() + format, args...);
    }

    template<typename... Args>
    static void debug(const std::string& loggerName, const std::string& format, Args&&... args) {
        assertInitialized();
        quartz::util::Logger::loggerPtrMap[loggerName]->debug(quartz::util::Logger::createIndentationString() + format, args...);
    }

    template<typename... Args>
    static void info(const std::string& loggerName, const std::string& format, Args&&... args) {
        assertInitialized();
        quartz::util::Logger::loggerPtrMap[loggerName]->info(quartz::util::Logger::createIndentationString() + format, args...);
    }

    template<typename... Args>
    static void warning(const std::string& loggerName, const std::string& format, Args&&... args) {
        assertInitialized();
        quartz::util::Logger::loggerPtrMap[loggerName]->warn(quartz::util::Logger::createIndentationString() + format, args...);
    }

    template<typename... Args>
    static void error(const std::string& loggerName, const std::string& format, Args&&... args) {
        assertInitialized();
        quartz::util::Logger::loggerPtrMap[loggerName]->error(quartz::util::Logger::createIndentationString() + format, args...);
    }

    template<typename... Args>
    static void critical(const std::string& loggerName, const std::string& format, Args&&... args) {
        assertInitialized();
        quartz::util::Logger::loggerPtrMap[loggerName]->critical(quartz::util::Logger::createIndentationString() + format, args...);
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
    static void log(const std::string& loggerName, const quartz::util::Logger::Level level, const std::string& format, Args&&... args) {
        switch (level) {
            case quartz::util::Logger::Level::trace:
                quartz::util::Logger::trace(loggerName, format, args...);
                break;
            case quartz::util::Logger::Level::debug:
                quartz::util::Logger::debug(loggerName, format, args...);
                break;
            case quartz::util::Logger::Level::info:
                quartz::util::Logger::info(loggerName, format, args...);
                break;
            case quartz::util::Logger::Level::warning:
                quartz::util::Logger::warning(loggerName, format, args...);
                break;
            case quartz::util::Logger::Level::error:
                quartz::util::Logger::error(loggerName, format, args...);
                break;
            case quartz::util::Logger::Level::critical:
                quartz::util::Logger::critical(loggerName, format, args...);
                break;
        }
    }

public: // public member functions
    Logger() = delete;

private: // private static functions
    static void init();
    static void assertInitialized();

    static quartz::util::Logger::Level getLevel(const std::string& loggerName);

    static std::string createIndentationString();

private: // private static variables
    static bool initialized;

    static std::map<std::string, quartz::util::Logger::Level> loggerNameLevelMap;

    static std::shared_ptr<spdlog::details::thread_pool> threadPool;
    static std::vector<spdlog::sink_ptr> sinkPtrs;
    static std::map<std::string, std::shared_ptr<quartz::util::spdlog_logger_t>> loggerPtrMap;
};

/**
 * @brief Log something
 * 
 * @note To use this LOGGER_NAME must be defined and the first argument of the variadic
 * arguments must be a formatting string
 */

#define LOG_TRACE(...) \
    quartz::util::Logger::trace(LOGGER_NAME, __VA_ARGS__)

#define LOG_DEBUG(...) \
    quartz::util::Logger::debug(LOGGER_NAME, __VA_ARGS__)

#define LOG_INFO(...) \
    quartz::util::Logger::info(LOGGER_NAME, __VA_ARGS__)

#define LOG_WARNING(...) \
    quartz::util::Logger::warning(LOGGER_NAME, __VA_ARGS__)
    
#define LOG_ERROR(...) \
    quartz::util::Logger::error(LOGGER_NAME, __VA_ARGS__)

#define LOG_CRITICAL(...) \
    quartz::util::Logger::critical(LOGGER_NAME, __VA_ARGS__)

/**
 * @brief Log a scope change
 * 
 * @note To use this LOGGER_NAME must be defined
 */

#define LOG_SCOPE_CHANGE_TRACE() \
    const quartz::util::Logger::Scoper UNIQUE_NAME(scoper)(LOGGER_NAME, quartz::util::Logger::Level::trace)

#define LOG_SCOPE_CHANGE_DEBUG() \
    const quartz::util::Logger::Scoper UNIQUE_NAME(scoper)(LOGGER_NAME, quartz::util::Logger::Level::debug)

#define LOG_SCOPE_CHANGE_INFO() \
    const quartz::util::Logger::Scoper UNIQUE_NAME(scoper)(LOGGER_NAME, quartz::util::Logger::Level::info)

#define LOG_SCOPE_CHANGE_WARNING() \
    const quartz::util::Logger::Scoper UNIQUE_NAME(scoper)(LOGGER_NAME, quartz::util::Logger::Level::warning)

#define LOG_SCOPE_CHANGE_ERROR() \
    const quartz::util::Logger::Scoper UNIQUE_NAME(scoper)(LOGGER_NAME, quartz::util::Logger::Level::error)

#define LOG_SCOPE_CHANGE_CRITICAL() \
    const quartz::util::Logger::Scoper UNIQUE_NAME(scoper)(LOGGER_NAME, quartz::util::Logger::Level::critical)

/**
 * @brief log a function call *WITHOUT* scope change
 * 
 * @note To use this LOGGER_NAME must be defined
 */

#define LOG_FUNCTION_CALL_TRACE(format, ...) \
    quartz::util::Logger::trace(LOGGER_NAME, __PRETTY_FUNCTION__ + std::string(" [ ") + format + std::string(" ]"), __VA_ARGS__)

#define LOG_FUNCTION_CALL_DEBUG(format, ...) \
    quartz::util::Logger::debug(LOGGER_NAME, __PRETTY_FUNCTION__ + std::string(" [ ") + format + std::string(" ]"), __VA_ARGS__)

#define LOG_FUNCTION_CALL_INFO(format, ...) \
    quartz::util::Logger::info(LOGGER_NAME, __PRETTY_FUNCTION__ + std::string(" [ ") + format + std::string(" ]"), __VA_ARGS__)

#define LOG_FUNCTION_CALL_WARNING(format, ...) \
    quartz::util::Logger::warning(LOGGER_NAME, __PRETTY_FUNCTION__ + std::string(" [ ") + format + std::string(" ]"), __VA_ARGS__)

#define LOG_FUNCTION_CALL_ERROR(format, ...) \
    quartz::util::Logger::error(LOGGER_NAME, __PRETTY_FUNCTION__ + std::string(" [ ") + format + std::string(" ]"), __VA_ARGS__)

#define LOG_FUNCTION_CALL_CRITICAL(format, ...) \
    quartz::util::Logger::critical(LOGGER_NAME, __PRETTY_FUNCTION__ + std::string(" [ ") + format + std::string(" ]"), __VA_ARGS__)

/**
 * @brief log a function call and its scope change
 * 
 * @note To use this LOGGER_NAME must be defined
 */

#define LOG_FUNCTION_SCOPE_TRACE(format, ...) \
    LOG_FUNCTION_CALL_TRACE(format, __VA_ARGS__); \
    LOG_SCOPE_CHANGE_TRACE()
    
#define LOG_FUNCTION_SCOPE_DEBUG(format, ...) \
    LOG_FUNCTION_CALL_DEBUG(format, __VA_ARGS__); \
    LOG_SCOPE_CHANGE_DEBUG()
    
#define LOG_FUNCTION_SCOPE_INFO(format, ...) \
    LOG_FUNCTION_CALL_INFO(format, __VA_ARGS__); \
    LOG_SCOPE_CHANGE_INFO()
    
#define LOG_FUNCTION_SCOPE_WARNING(format, ...) \
    LOG_FUNCTION_CALL_WARNING(format, __VA_ARGS__); \
    LOG_SCOPE_CHANGE_WARNING()
    
#define LOG_FUNCTION_SCOPE_ERROR(format, ...) \
    LOG_FUNCTION_CALL_ERROR(format, __VA_ARGS__); \
    LOG_SCOPE_CHANGE_ERROR()
    
#define LOG_FUNCTION_SCOPE_CRITICAL(format, ...) \
    LOG_FUNCTION_CALL_CRITICAL(format, __VA_ARGS__); \
    LOG_SCOPE_CHANGE_CRITICAL()
    