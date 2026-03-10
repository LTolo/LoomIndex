#pragma once

#include <string>
#include <mutex>

namespace loom {

enum class LogLevel {
    INFO,
    WARNING,
    ERROR
};

/**
 * @class Logger
 * @brief Thread-safe simple logger for LoomIndex.
 * Supports timestamping and different log levels to stdout/stderr.
 */
class Logger {
public:
    static Logger& get_instance() {
        static Logger instance;
        return instance;
    }

    // Prevent copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void log(LogLevel level, const std::string& message);
    
    // Convenience wrappers
    static void info(const std::string& message) { get_instance().log(LogLevel::INFO, message); }
    static void warning(const std::string& message) { get_instance().log(LogLevel::WARNING, message); }
    static void error(const std::string& message) { get_instance().log(LogLevel::ERROR, message); }

private:
    Logger() = default;
    ~Logger() = default;

    std::mutex log_mutex_;
    std::string get_current_timestamp() const;
    std::string level_to_string(LogLevel level) const;
};

} // namespace loom
