#include "LoomIndex/CrawlerEngine.hpp"
#include "LoomIndex/Logger.hpp"
#include <regex>
#include <chrono>

namespace loom {

CrawlerEngine::CrawlerEngine(std::size_t num_threads, const std::vector<std::string>& seed_urls)
    : num_threads_(num_threads), 
      frontier_(std::make_shared<URLFrontier>()),
      fetcher_(std::make_unique<AsyncFetcher>()) 
{
    Logger::info("Initializing CrawlerEngine with " + std::to_string(num_threads) + " worker threads.");
    for (const auto& url : seed_urls) {
        frontier_->push_if_new(url);
    }
}

CrawlerEngine::~CrawlerEngine() {
    stop();
}

void CrawlerEngine::start() {
    if (is_running_) return;
    is_running_ = true;
    Logger::info("CrawlerEngine started.");

    // Launch parsing/processing worker threads
    for (std::size_t i = 0; i < num_threads_; ++i) {
        workers_.emplace_back(&CrawlerEngine::worker_loop, this);
    }

    // Main thread acts as the IO event loop.
    // In a massive system, this might be multiple threads pumping curl_multi_wait.
    while (is_running_) {
        fetcher_->perform_io();
        // Give some CPU back, realistic systems use epoll/select via curl_multi_wait
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    Logger::info("CrawlerEngine IO loop terminated.");
}

void CrawlerEngine::stop() {
    if (!is_running_) return;
    Logger::info("Initiating graceful shutdown of CrawlerEngine...");
    
    is_running_ = false;
    frontier_->shutdown();
    
    Logger::info("Waiting for target worker threads to finish...");
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    Logger::info("Graceful shutdown complete.");
}

void CrawlerEngine::worker_loop() {
    while (is_running_) {
        // Blocks until URL is available or shutdown
        auto optional_url = frontier_->pop_wait();
        if (!optional_url) {
            Logger::info("Worker thread exiting cleanly: Queue shutdown detected.");
            break; // Shutdown signaled
        }

        std::string url = *optional_url;
        Logger::info("Worker processing URL: " + url);
        
        // Push IO request to AsyncFetcher
        // In a strictly memory-safe cross-thread scenario, fetcher_ needs mutual exclusion
        // around fetch_async if the libcurl handle isn't thread safe for insertion during perform_io.
        // For demonstration, we assume fetch_async handles it or lock contention is acceptable.
        
        auto callback = [this](FetchResult result) {
            this->on_page_fetched(std::move(result));
        };

        fetcher_->fetch_async(url, std::move(callback));
    }
}

void CrawlerEngine::on_page_fetched(const FetchResult& result) {
    if (!result.success) {
        // Error already logged by AsyncFetcher, but we can aggregate here
        return;
    }

    urls_processed_++;
    Logger::info("Successfully fetched: " + result.url + " (" + std::to_string(result.body.size()) + " bytes)");

    // Very naive link extraction
    std::regex link_regex(R"(href=\"(http[s]?://[^\"]+)\")");
    auto words_begin = std::sregex_iterator(result.body.begin(), result.body.end(), link_regex);
    auto words_end = std::sregex_iterator();

    std::size_t new_links_count = 0;
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        std::string new_url = match.str(1);
        if (frontier_->push_if_new(new_url)) {
            new_links_count++;
        }
    }
    
    if (new_links_count > 0) {
        Logger::info("Extracted " + std::to_string(new_links_count) + " NEW unique links from " + result.url);
    }
}

} // namespace loom
