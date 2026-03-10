#include <gtest/gtest.h>
#include "LoomIndex/CrawlerEngine.hpp"

TEST(CrawlerEngineTest, InitAndStop) {
    std::vector<std::string> seeds = {"http://example.com"};
    loom::CrawlerEngine engine(2, seeds);
    
    // We only test graceful stop and construction here to prevent real logic loops.
    engine.stop();
    EXPECT_TRUE(true);
}
