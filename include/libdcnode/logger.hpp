/*
* Copyright (C) 2023 Anastasiia Stepanova <asiiapine@gmail.com>
* Distributed under the terms of the GPL v3 license, available in the file LICENSE.
*/

#ifndef LIBDCNODE_LOGGER_HPP_
#define LIBDCNODE_LOGGER_HPP_

#include <cstdio>
#include "libdcnode/uavcan/protocol/debug/LogMessage.h"

class DronecanLogger {
public:
    /**
     * @brief Instanciate a LogMessage publisher.
     * @param[in] source name should be as sort as possible. The maximum length is 31 symbols.
     * @note If you can't give a source name on the instantiation time, you can call init later.
     */
    explicit DronecanLogger(const char* source = "");

    /**
     * @brief This function is optional.
     * Use it only when you can't assign a name in the constructor or want to update the name.
     */
    void init(const char* source);

    /**
     * @brief Use these functions if you know the logging level on compile time:
     * Debug: is applied only if your build type is Debug. It is not compiled in Release builds.
     * Info/Warn: something that can be ignored or printed by the application severity level.
     * Error: always print.
     */
    void log_debug(const char* text);
    void log_info(const char* text);
    void log_warn(const char* text);
    void log_error(const char* text);

    /**
     * @brief Alternatively, you this if you want specifying the logging level in real time.
     * @param[in] severity The severity level of the message.
     * @param[in] text The message to log.
     */
    void log(uint8_t severity, const char* text);

    /**
     * @brief Global logging function that allows specifying severity, source, and text.
     * @param[in] severity The severity level of the message.
     * @param[in] source The source of the message.
     * @param[in] text The message to log.
     */
    static void log_global(uint8_t severity, const char* source, const char* text);

private:
    static void publish(const DebugLogMessage_t& msg);

    // Transfer ID is the same for all instances
    static inline uint8_t _transfer_id;

    DebugLogMessage_t _msg;
};

#endif  // LIBDCNODE_LOGGER_HPP_
