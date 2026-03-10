#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include "LoomIndex/CrawlerEngine.hpp"
#include "LoomIndex/Logger.hpp"

int main() {
    loom::Logger::info("Starting LoomIndex Crawler Demo...");
    
    std::vector<std::string> seeds = {
        "http://example.com"
    };
    
    // Engine mit 2 Worker-Threads initialisieren
    loom::CrawlerEngine engine(2, seeds);
    
    // Engine in separatem Thread starten, um sie nach X Sekunden stoppen zu können
    std::thread engine_thread([&engine]() {
        engine.start();
    });
    
    // Lass den Crawler für 3 Sekunden laufen
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    loom::Logger::info("Stopping LoomIndex Crawler Demo...");
    engine.stop();
    engine_thread.join();
    
    loom::Logger::info("Demo Shutdown Complete.");
    return 0;
}
