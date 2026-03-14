#include "LoomIndex/Logger.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace loom {

std::string Logger::get_current_timestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Logger::level_to_string(LogLevel level) const {
    switch (level) {
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARN";
        case LogLevel::ERROR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

void Logger::log(LogLevel level, const std::string& message) const {
    std::lock_guard<std::mutex> lock(log_mutex_);
    
    std::string prefix = "[" + get_current_timestamp() + "] [" + level_to_string(level) + "] ";
    
    if (level == LogLevel::ERROR) {
        std::cerr << prefix << message << std::endl;
    } else {
        std::cout << prefix << message << std::endl;
    }
}

} // namespace loom
