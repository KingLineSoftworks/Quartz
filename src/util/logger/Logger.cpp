#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

#include "spdlog/async.h"
#include "spdlog/sinks/dist_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "util/logger/Logger.hpp"

/* ------------------------------ public static variables ------------------------------ */

/* ------------------------------ private static variables ------------------------------ */

/**
 * @brief A variable for us to determine how many indentaions we want to use for the message
 * we are logging
 */
uint32_t quartz::util::Logger::Scoper::indentationCount = 0;

/**
 * @brief The map that is tracking the corresponding *default* logging levels for each of the loggers
 */
std::map<std::string, quartz::util::Logger::Level> quartz::util::Logger::loggerNameDefaultLevelMap;

/**
 * @brief The map that is tracking the corresponding *current* logging levels for each of the loggers
 */
std::map<std::string, quartz::util::Logger::Level> quartz::util::Logger::loggerNameLevelMap;

/**
 * @brief A flag representing whether or not the spdlog stuff has been initialized
 */
bool quartz::util::Logger::initialized = false;

/**
 * @brief The underlying thread pool that spdlog uses for the asynchronous logging
 */
std::shared_ptr<spdlog::details::thread_pool> quartz::util::Logger::threadPool = nullptr;

/**
 * @brief The spdlog sink pointers we initialize and givse to each of our loggers
 */
std::vector<spdlog::sink_ptr> quartz::util::Logger::sinkPtrs;

/**
 * @brief The map that is actually tracking the spdlog loggers we're using based on
 * the strings representing the logger names
 */
std::map<std::string, std::shared_ptr<quartz::util::spdlog_logger_t>> quartz::util::Logger::loggerPtrMap;

/* ------------------------------ public static functions ------------------------------ */

/**
 * @brief Register the logger of the desired name
 */
void quartz::util::Logger::registerLogger(const std::string& loggerName, const quartz::util::Logger::Level defaultLevel) {
    if (!quartz::util::Logger::initialized) {
        quartz::util::Logger::init();
    }

    // Create the spdlog logger we are going to use
    // In debug mode we use a regular (non asynchronous) logger
    // In test and release mode we use an asynchronous logger
#if defined QUARTZ_DEBUG
    std::shared_ptr<quartz::util::spdlog_logger_t> p_logger = std::make_shared<quartz::util::spdlog_logger_t>(
        loggerName,
        quartz::util::Logger::sinkPtrs.begin(),
        quartz::util::Logger::sinkPtrs.end()
    );
#else
    std::shared_ptr<quartz::util::spdlog_logger_t> p_logger = std::make_shared<quartz::util::spdlog_logger_t>(
        loggerName,
        quartz::util::Logger::sinkPtrs.begin(),
        quartz::util::Logger::sinkPtrs.end(),
        quartz::util::Logger::threadPool,
        spdlog::async_overflow_policy::block
    );
#endif

    /**
     * @todo Use the DEBUG, TEST, RELEASE macros to clamp the logging levels to
     * a certain minimum value?
     */

    // Set the logging level
    switch (defaultLevel) {
        case quartz::util::Logger::Level::trace:
            p_logger->set_level(spdlog::level::trace);
            break;
        case quartz::util::Logger::Level::debug:
            p_logger->set_level(spdlog::level::debug);
            break;
        case quartz::util::Logger::Level::info:
            p_logger->set_level(spdlog::level::info);
            break;
        case quartz::util::Logger::Level::warning:
            p_logger->set_level(spdlog::level::warn);
            break;
        case quartz::util::Logger::Level::error:
            p_logger->set_level(spdlog::level::err);
            break;
        case quartz::util::Logger::Level::critical:
            p_logger->set_level(spdlog::level::critical);
            break;
        case quartz::util::Logger::Level::off:
            p_logger->set_level(spdlog::level::off);
            break;
    }

    spdlog::register_logger(p_logger);

    // Set the format for the sinks
    spdlog::set_pattern("[%T:%e] [%-10!n] [%^%-8l%$] %v");

    quartz::util::Logger::loggerNameDefaultLevelMap[loggerName] = defaultLevel;
    quartz::util::Logger::loggerNameLevelMap[loggerName] = defaultLevel;
    quartz::util::Logger::loggerPtrMap[loggerName] = p_logger;
    
    quartz::util::Logger::log(loggerName, defaultLevel, "Logger {} initialized with default level of {}", loggerName, static_cast<uint32_t>(defaultLevel));
}

/**
 * @brief Update the logging level for the desired logger. Only update this level if it is more exclusive than its default logging level.
 */
void quartz::util::Logger::setLevel(const std::string& loggerName, const quartz::util::Logger::Level desiredLevel) {
    std::shared_ptr<quartz::util::spdlog_logger_t> p_logger = quartz::util::Logger::loggerPtrMap[loggerName];
    const quartz::util::Logger::Level defaultLevel = quartz::util::Logger::loggerNameDefaultLevelMap[loggerName];

    if (desiredLevel <= defaultLevel) {
        p_logger->critical("Not setting Logger {} to deired level of {} because it is not greater than its default level of {}", loggerName, static_cast<uint32_t>(desiredLevel), static_cast<uint32_t>(defaultLevel));
        return;
    }

    const quartz::util::Logger::Level previousLevel = quartz::util::Logger::loggerNameLevelMap[loggerName];

    // Actually set the underlying logging level
    switch (desiredLevel) {
        case quartz::util::Logger::Level::trace:
            p_logger->set_level(spdlog::level::trace);
            break;
        case quartz::util::Logger::Level::debug:
            p_logger->set_level(spdlog::level::debug);
            break;
        case quartz::util::Logger::Level::info:
            p_logger->set_level(spdlog::level::info);
            break;
        case quartz::util::Logger::Level::warning:
            p_logger->set_level(spdlog::level::warn);
            break;
        case quartz::util::Logger::Level::error:
            p_logger->set_level(spdlog::level::err);
            break;
        case quartz::util::Logger::Level::critical:
            p_logger->set_level(spdlog::level::critical);
            break;
        case quartz::util::Logger::Level::off:
            p_logger->set_level(spdlog::level::off);
            break;
    }

    quartz::util::Logger::loggerNameLevelMap[loggerName] = desiredLevel;

    p_logger->critical("Successfully set Logger {} to desired level of {} (previous level was {}, default level is {})", loggerName, static_cast<uint32_t>(desiredLevel), static_cast<uint32_t>(previousLevel), static_cast<uint32_t>(defaultLevel));
}

/**
 * @brief Attempt to update the logging levels for all of the desired loggers
 */
void quartz::util::Logger::setLevels(const std::vector<const quartz::util::Logger::RegistrationInfo>& loggerInfos) {
    for (const quartz::util::Logger::RegistrationInfo& loggerInfo : loggerInfos) {
        quartz::util::Logger::setLevel(loggerInfo.loggerName, loggerInfo.level);
    }
}

/* ------------------------------ private static functions ------------------------------ */

/**
 * @brief A function allowing us to initialize the logger so we don't need to do a
 * conditional check for initialization everytime we want to log something
 * time we want to 
 * 
 * @details Create stdout sink to receive all logs.
 * In debug mode this is single threaded.
 * In test mode this is multi threaded.
 * In release mode this is not used.
 * 
 * @details Create file sink.
 * In debug mode this receives all logs.
 * In test and release mode this only receives logs at level warning or higher.
 */
void quartz::util::Logger::init() {
    const uint32_t threadPoolBackingItems = 8192;
    const uint32_t threadPoolBackingThreadCount = 1;
    spdlog::init_thread_pool(threadPoolBackingItems, threadPoolBackingThreadCount);
    quartz::util::Logger::threadPool = spdlog::thread_pool();

    // Create the stdout sink and add it to the vector of sinks that we give to each of the loggers
    // Use trace as max detail (allow all types to go to stdout log)
    // Use a synchronous (single threaded) logger in debug mode
    // Use a multithreaded logger in test mode
#if defined QUARTZ_DEBUG
    spdlog::sink_ptr p_stdoutSink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
    p_stdoutSink->set_level(spdlog::level::trace);
    quartz::util::Logger::sinkPtrs.push_back(p_stdoutSink);
#elif defined QUARTZ_TEST
    spdlog::sink_ptr p_stdoutSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    p_stdoutSink->set_level(spdlog::level::trace);
    quartz::util::Logger::sinkPtrs.push_back(p_stdoutSink);
#endif

    // Create the file sink with the name determined by the current date and time
    // If we are in test or release mode then use warnings as max detail by default (only warnings, errors, and critical go to file log)
    // If we are in debug mode then allow all log statements to go to the log files so we have persistent storage of all logs
    time_t time = std::time(nullptr);
    struct tm* p_currTime = std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(p_currTime, "%Y.%m.%d.%H.%M.%S");
    std::string dateString = oss.str();
    std::string fileName = "QUARTZlog." + dateString + ".log";
    spdlog::sink_ptr p_fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(fileName, true);
    p_fileSink->set_level(
#if defined QUARTZ_DEBUG
        spdlog::level::trace
#else
        spdlog::level::warn
#endif
    );

    // Put both the file sink into the vector of sinks because we always want console logging
    quartz::util::Logger::sinkPtrs.push_back(p_fileSink);

    // Now that our sink pointers are initialized we are good to go
    quartz::util::Logger::initialized = true;
}

/**
 * @brief A function allowing us to ensure that we have been initialized prior to attempting
 * to log anything. If we are in release mode we do nothing to save clock cycles
 * 
 * @todo Should we be doing this? Should we use c++'s assert instead?
 */
void quartz::util::Logger::assertInitialized() {
#if !defined QUARTZ_RELEASE
    if (!quartz::util::Logger::initialized) {
        std::string initializationErrorMessage = "quartz::util::Logger not initialized";
        std::cerr << initializationErrorMessage << "\n";
        throw std::runtime_error(initializationErrorMessage);
    }
#endif
}

/**
 * @brief Get the level of the logger with the corresponding name. If no logger is found with this
 * name then an exception will be thrown
 * 
 * @param loggerName The name of the logger to get the level for
 * @return quartz::util::Logger::Level The level of the logger
 */
quartz::util::Logger::Level quartz::util::Logger::getLevel(const std::string& loggerName) {
    if (quartz::util::Logger::loggerNameLevelMap.count(loggerName) <= 0) {
        std::string levelErrorMessage = "No quartz::util::Logger found with name " + loggerName;
        #if defined(BUILD_DEBUG) || defined(BUILD_TEST)
        std::cerr << levelErrorMessage << "\n";
        #endif
        throw std::runtime_error(levelErrorMessage);
    }

    return quartz::util::Logger::loggerNameLevelMap[loggerName];
}

/**
 * @brief Create a string to prepend to the message based on how many indents we
 * should be using
 * 
 * @return std::string The whitespace string 
 */
std::string quartz::util::Logger::createIndentationString() {
    return std::string(quartz::util::Logger::Scoper::getIndentationCount() * 4, ' ');
}

/* ------------------------------ public member functions ------------------------------ */

/**
 * @brief Construct a new quartz::util::Logger::Scoper::Scoper object to denote a change
 * in scope when we are logging. This logs an opening curly brace and increments
 * the indentation counter so we know how much whitespace to print when logging
 * a message
 * 
 * @param level The level at which to log the opening/closing braces
 */
quartz::util::Logger::Scoper::Scoper(const std::string& loggerName, const quartz::util::Logger::Level level) :
    m_loggerName(loggerName),
    m_level(level)
{
    if (m_level >= quartz::util::Logger::getLevel(m_loggerName)) {
        quartz::util::Logger::log(m_loggerName, m_level, "{");
        quartz::util::Logger::Scoper::indentationCount++;
    }
}

/**
 * @brief Destroy the quartz::util::Logger::Scoper::Scoper object to denote that a scope
 * is ending. Decrement the indentation count and log a closing curly brace to denote
 * this
 */
quartz::util::Logger::Scoper::~Scoper() {
    if (m_level >= quartz::util::Logger::getLevel(m_loggerName)) {
        quartz::util::Logger::Scoper::indentationCount--;
        quartz::util::Logger::log(m_loggerName, m_level, "}");
    }
}

/* ------------------------------ private member functions ------------------------------ */
