#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include "LoomIndex/CrawlerEngine.hpp"
#include "LoomIndex/Logger.hpp"

int main(int argc, char* argv[]) {
    loom::Logger::info("LoomIndex v1.0 Starting...");
    
    std::vector<std::string> seeds;
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            seeds.push_back(argv[i]);
        }
    } else {
        seeds.push_back("https://example.com");
    }
    
    // Initialize engine with 2 worker threads
    loom::CrawlerEngine engine(2, seeds);
    
    // Start engine in a separate thread so it can be stopped after X seconds
    std::thread engine_thread([&engine]() {
        engine.start();
    });
    
    // Let the crawler run for 3 seconds
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    loom::Logger::info("Stopping LoomIndex Crawler Demo...");
    engine.stop();
    engine_thread.join();
    
    loom::Logger::info("Crawl finished. " + std::to_string(engine.get_urls_processed()) + " URLs processed.");
    
    return 0;
}
