#include <gtest/gtest.h>
#include "LoomIndex/AsyncFetcher.hpp"

// Requires libcurl to be correctly linked and ideally initialized globally.
TEST(AsyncFetcherTest, InitAndCleanup) {
    loom::AsyncFetcher fetcher;
    // Should pass without crash if curl multi handles allocate correctly.
    EXPECT_TRUE(true);
}
