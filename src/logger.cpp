#include "logger.h"

#include <plog/Log.h>

/**
 * @brief Logger::init
 * @param path the log file path
 */
void Logger::init(const QString &path)
{
    plog::init(plog::debug, path.toUtf8().data());
}

/**
 * @brief Logger::debug
 * @param msg the message to be logged
 */
void Logger::debug(const QString &msg)
{
    LOG_DEBUG << msg;
}

/**
 * @brief Logger::info
 * @param msg the message to be logged
 */
void Logger::info(const QString &msg)
{
    LOG_INFO << msg;
}

/**
 * @brief Logger::warning
 * @param msg the message to be logged
 */
void Logger::warning(const QString &msg)
{
    LOG_WARNING << msg;
}

/**
 * @brief Logger::error
 * @param msg the message to be logged
 */
void Logger::error(const QString &msg)
{
    LOG_ERROR << msg;
}
