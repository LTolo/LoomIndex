#pragma once

#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <optional>
#include "loomindex/BloomFilter.hpp"

namespace loom {

/**
 * @class URLFrontier
 * @brief Thread-safe queue for managing URLs to be crawled.
 * Integrates BloomFilter to discard duplicates gracefully.
 */
class URLFrontier {
public:
    /**
     * @brief Constructs the URLFrontier with deduplication enabled.
     * @param expected_urls Used to initialize the internal Bloom Filter.
     */
    explicit URLFrontier(std::size_t expected_urls = 1000000);

    ~URLFrontier() = default;

    URLFrontier(const URLFrontier&) = delete;
    URLFrontier& operator=(const URLFrontier&) = delete;

    /**
     * @brief Pushes a new URL. Will not push if it's already visited (BloomFilter check).
     * @param url The URL to add.
     * @return true if added, false if it was rejected as a duplicate.
     */
    bool push_if_new(const std::string& url);

    /**
     * @brief Pops a URL from the frontier. Blocks if the queue is empty.
     * @return The popped URL. If nullopt, means frontier is done/terminated.
     */
    std::optional<std::string> pop_wait();

    /**
     * @brief Gracefully terminates waiting threads.
     */
    void shutdown();

    /**
     * @brief Gets current approximate size.
     */
    [[nodiscard]] std::size_t size() const;

private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<std::string> queue_;
    
    // std::unique_ptr for RAII and heap allocation of the potentially large BloomFilter
    std::unique_ptr<BloomFilter> seen_urls_;
    bool shutdown_flag_{false};
};

} // namespace loom
