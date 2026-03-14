#pragma once

#include <memory>
#include <thread>
#include <vector>
#include <string>
#include <atomic>

#include "LoomIndex/URLFrontier.hpp"
#include "LoomIndex/AsyncFetcher.hpp"

namespace loom {

/**
 * @class CrawlerEngine
 * @brief Main execution engine for the LoomIndex crawler.
 * Coordinates the thread pool, fetches URLs using AsyncFetcher, and queues newly discovered URLs.
 */
class CrawlerEngine {
public:
    /**
     * @brief Configures the engine.
     * @param num_threads Number of worker threads for parsing/processing.
     * @param seed_urls Initial URLs to bootstrap the crawler.
     */
    CrawlerEngine(std::size_t num_threads, const std::vector<std::string>& seed_urls);

    ~CrawlerEngine();

    CrawlerEngine(const CrawlerEngine&) = delete;
    CrawlerEngine& operator=(const CrawlerEngine&) = delete;

    /**
     * @brief Starts the crawling process. Blocks until crawling is complete or stopped.
     */
    void start();

    /**
     * @brief Gracefully requests the crawler to halt.
     */
    void stop();

    /**
     * @brief Returns the number of successfully processed URLs.
     */
    [[nodiscard]] std::size_t get_urls_processed() const {
        return urls_processed_.load();
    }

private:
    std::size_t num_threads_;
    std::atomic<bool> is_running_{false};
    std::atomic<std::size_t> urls_processed_{0};
    
    std::shared_ptr<URLFrontier> frontier_;
    std::unique_ptr<AsyncFetcher> fetcher_; // Or one per thread depending on architecture
    std::vector<std::thread> workers_;

    // Internal loop for worker threads
    void worker_loop();

    // Internal callback for when fetcher finishes downloading a page
    void on_page_fetched(const FetchResult& result);
};

} // namespace loom
