#include "LoomIndex/URLFrontier.hpp"

namespace loom {

URLFrontier::URLFrontier(std::size_t expected_urls) 
    : seen_urls_(std::make_unique<BloomFilter>(expected_urls, 0.01)) {}

bool URLFrontier::push_if_new(const std::string& url) {
    std::unique_lock<std::mutex> lock(mutex_);
    
    if (shutdown_flag_) {
        return false;
    }
    
    if (seen_urls_->possibly_contains(url)) {
        // Logically it might be a false positive, but for a simple crawler we accept skipping it.
        return false;
    }
    
    seen_urls_->add(url);
    queue_.push(url);
    lock.unlock();
    cv_.notify_one();
    
    return true;
}

std::optional<std::string> URLFrontier::pop_wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return !queue_.empty() || shutdown_flag_; });
    
    if (shutdown_flag_ && queue_.empty()) {
        return std::nullopt;
    }
    
    std::string url = std::move(queue_.front());
    queue_.pop();
    return url;
}

void URLFrontier::shutdown() {
    std::unique_lock<std::mutex> lock(mutex_);
    shutdown_flag_ = true;
    lock.unlock();
    cv_.notify_all();
}

std::size_t URLFrontier::size() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return queue_.size();
}

} // namespace loom
