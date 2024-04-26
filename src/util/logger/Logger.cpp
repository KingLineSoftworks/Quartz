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
uint32_t util::Logger::Scoper::indentationCount = 0;

/**
 * @brief The map that is tracking the corresponding *default* logging levels for each of the loggers
 */
std::map<std::string, util::Logger::Level> util::Logger::loggerNameDefaultLevelMap;

/**
 * @brief The map that is tracking the corresponding *current* logging levels for each of the loggers
 */
std::map<std::string, util::Logger::Level> util::Logger::loggerNameLevelMap;

/**
 * @brief A flag representing whether or not the spdlog stuff has been initialized
 */
bool util::Logger::initialized = false;

/**
 * @brief A flag representing whether or not the preamble should be logged.
 * This is when we log information about registering loggers and changing their logging level
 */
bool util::Logger::shouldLogPreamble = true;

/**
 * @brief The underlying thread pool that spdlog uses for the asynchronous logging
 */
std::shared_ptr<spdlog::details::thread_pool> util::Logger::threadPool = nullptr;

/**
 * @brief The spdlog sink pointers we initialize and givse to each of our loggers
 */
std::vector<spdlog::sink_ptr> util::Logger::sinkPtrs;

/**
 * @brief The map that is actually tracking the spdlog loggers we're using based on
 * the strings representing the logger names
 */
std::map<std::string, std::shared_ptr<util::spdlog_logger_t>> util::Logger::loggerPtrMap;

/* ------------------------------ public static functions ------------------------------ */

/**
 * @brief Register the logger of the desired name
 */
void util::Logger::registerLogger(const std::string& loggerName, const util::Logger::Level defaultLevel) {
    if (!util::Logger::initialized) {
        util::Logger::init();
    }

    // Create the spdlog logger we are going to use
    // In debug mode we use a regular (non asynchronous) logger
    // In test and release mode we use an asynchronous logger
#if defined QUARTZ_DEBUG
    std::shared_ptr<util::spdlog_logger_t> p_logger = std::make_shared<util::spdlog_logger_t>(
        loggerName,
        util::Logger::sinkPtrs.begin(),
        util::Logger::sinkPtrs.end()
    );
#else
    std::shared_ptr<util::spdlog_logger_t> p_logger = std::make_shared<util::spdlog_logger_t>(
        loggerName,
        util::Logger::sinkPtrs.begin(),
        util::Logger::sinkPtrs.end(),
        util::Logger::threadPool,
        spdlog::async_overflow_policy::block
    );
#endif

    /**
     * @todo 2023/10/20 Use the DEBUG, TEST, RELEASE macros to clamp the logging levels to
     * a certain minimum value?
     */

    // Set the logging level
    switch (defaultLevel) {
        case util::Logger::Level::trace:
            p_logger->set_level(spdlog::level::trace);
            break;
        case util::Logger::Level::debug:
            p_logger->set_level(spdlog::level::debug);
            break;
        case util::Logger::Level::info:
            p_logger->set_level(spdlog::level::info);
            break;
        case util::Logger::Level::warning:
            p_logger->set_level(spdlog::level::warn);
            break;
        case util::Logger::Level::error:
            p_logger->set_level(spdlog::level::err);
            break;
        case util::Logger::Level::critical:
            p_logger->set_level(spdlog::level::critical);
            break;
        case util::Logger::Level::off:
            p_logger->set_level(spdlog::level::off);
            break;
    }

    spdlog::register_logger(p_logger);

    // Set the format for the sinks
    spdlog::set_pattern("[%T:%e] [%-10!n] [%^%-8l%$] %v");

    util::Logger::loggerNameDefaultLevelMap[loggerName] = defaultLevel;
    util::Logger::loggerNameLevelMap[loggerName] = defaultLevel;
    util::Logger::loggerPtrMap[loggerName] = p_logger;

    if (util::Logger::shouldLogPreamble) {
        util::Logger::log(loggerName, defaultLevel, "Logger {} initialized with default level of {}", loggerName, static_cast<uint32_t>(defaultLevel));
    }
}

/**
 * @brief Update the logging level for the desired logger. Only update this level if it is more exclusive than its default logging level.
 */
void util::Logger::setLevel(const std::string& loggerName, const util::Logger::Level desiredLevel) {
    std::shared_ptr<util::spdlog_logger_t> p_logger = util::Logger::loggerPtrMap[loggerName];
    const util::Logger::Level defaultLevel = util::Logger::loggerNameDefaultLevelMap[loggerName];
    const util::Logger::Level currentLevel = util::Logger::loggerNameLevelMap[loggerName];

    if (desiredLevel <= defaultLevel) {
        if (util::Logger::shouldLogPreamble) {
            util::Logger::log(loggerName, currentLevel, "Not setting Logger {} to desired level of {} because it is not greater than its default level of {}", loggerName, static_cast<uint32_t>(desiredLevel), static_cast<uint32_t>(defaultLevel));
        }
        return;
    }

    if (desiredLevel == currentLevel) {
        if (util::Logger::shouldLogPreamble) {
            util::Logger::log(loggerName, currentLevel, "Not setting Logger {} to desired level of {} because it is already set to that level", loggerName, static_cast<uint32_t>(desiredLevel));
        }
        return;
    }

    const util::Logger::Level previousLevel = util::Logger::loggerNameLevelMap[loggerName];

    // Actually set the underlying logging level
    switch (desiredLevel) {
        case util::Logger::Level::trace:
            p_logger->set_level(spdlog::level::trace);
            break;
        case util::Logger::Level::debug:
            p_logger->set_level(spdlog::level::debug);
            break;
        case util::Logger::Level::info:
            p_logger->set_level(spdlog::level::info);
            break;
        case util::Logger::Level::warning:
            p_logger->set_level(spdlog::level::warn);
            break;
        case util::Logger::Level::error:
            p_logger->set_level(spdlog::level::err);
            break;
        case util::Logger::Level::critical:
            p_logger->set_level(spdlog::level::critical);
            break;
        case util::Logger::Level::off:
            p_logger->set_level(spdlog::level::off);
            break;
    }

    util::Logger::loggerNameLevelMap[loggerName] = desiredLevel;

    if (util::Logger::shouldLogPreamble) {
        util::Logger::log(loggerName, desiredLevel, "Successfully set Logger {} to desired level of {} (previous level was {}, default level is {})", loggerName, static_cast<uint32_t>(desiredLevel), static_cast<uint32_t>(previousLevel), static_cast<uint32_t>(defaultLevel));
    }
}

/**
 * @brief Attempt to update the logging levels for all of the desired loggers
 */
void util::Logger::setLevels(const std::vector<const util::Logger::RegistrationInfo>& loggerInfos) {
    for (const util::Logger::RegistrationInfo& loggerInfo : loggerInfos) {
        util::Logger::setLevel(loggerInfo.loggerName, loggerInfo.level);
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
void util::Logger::init() {
    const uint32_t threadPoolBackingItems = 8192;
    const uint32_t threadPoolBackingThreadCount = 1;
    spdlog::init_thread_pool(threadPoolBackingItems, threadPoolBackingThreadCount);
    util::Logger::threadPool = spdlog::thread_pool();

    // Create the stdout sink and add it to the vector of sinks that we give to each of the loggers
    // Use trace as max detail (allow all types to go to stdout log)
    // Use a synchronous (single threaded) logger in debug mode
    // Use a multithreaded logger in test mode
#if defined QUARTZ_DEBUG
    spdlog::sink_ptr p_stdoutSink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
    p_stdoutSink->set_level(spdlog::level::trace);
    util::Logger::sinkPtrs.push_back(p_stdoutSink);
#elif defined QUARTZ_TEST
    spdlog::sink_ptr p_stdoutSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    p_stdoutSink->set_level(spdlog::level::trace);
    util::Logger::sinkPtrs.push_back(p_stdoutSink);
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
    util::Logger::sinkPtrs.push_back(p_fileSink);

    // Now that our sink pointers are initialized we are good to go
    util::Logger::initialized = true;
}

/**
 * @brief A function allowing us to ensure that we have been initialized prior to attempting
 * to log anything. If we are in release mode we do nothing to save clock cycles
 */
void util::Logger::assertInitialized() {
#if !defined QUARTZ_RELEASE
    if (!util::Logger::initialized) {
        std::string initializationErrorMessage = "util::Logger not initialized";
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
 * @return util::Logger::Level The level of the logger
 */
util::Logger::Level util::Logger::getLevel(const std::string& loggerName) {
    if (util::Logger::loggerNameLevelMap.count(loggerName) <= 0) {
        std::string levelErrorMessage = "No util::Logger found with name " + loggerName;
        #if defined(QUARTZ_DEBUG) || defined(QUARTZ_TEST)
        std::cerr << levelErrorMessage << "\n";
        #endif
        throw std::runtime_error(levelErrorMessage);
    }

    return util::Logger::loggerNameLevelMap[loggerName];
}

/**
 * @brief Create a string to prepend to the message based on how many indents we
 * should be using
 * 
 * @return std::string The whitespace string 
 */
std::string util::Logger::createIndentationString() {
    return std::string(util::Logger::Scoper::getIndentationCount() * 4, ' ');
}

/* ------------------------------ public member functions ------------------------------ */

/**
 * @brief Construct a new util::Logger::Scoper::Scoper object to denote a change
 * in scope when we are logging. This logs an opening curly brace and increments
 * the indentation counter so we know how much whitespace to print when logging
 * a message
 * 
 * @param level The level at which to log the opening/closing braces
 */
util::Logger::Scoper::Scoper(const std::string& loggerName, const util::Logger::Level level) :
    m_loggerName(loggerName),
    m_level(level)
{
    if (m_level >= util::Logger::getLevel(m_loggerName)) {
        util::Logger::log(m_loggerName, m_level, "{");
        util::Logger::Scoper::indentationCount++;
    }
}

/**
 * @brief Destroy the util::Logger::Scoper::Scoper object to denote that a scope
 * is ending. Decrement the indentation count and log a closing curly brace to denote
 * this
 */
util::Logger::Scoper::~Scoper() {
    if (m_level >= util::Logger::getLevel(m_loggerName)) {
        util::Logger::Scoper::indentationCount--;
        util::Logger::log(m_loggerName, m_level, "}");
    }
}

/* ------------------------------ private member functions ------------------------------ */
