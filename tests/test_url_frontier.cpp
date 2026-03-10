#include <gtest/gtest.h>
#include "LoomIndex/URLFrontier.hpp"
#include <thread>
#include <vector>

TEST(URLFrontierTest, PushAndPop) {
    loom::URLFrontier frontier(100);
    EXPECT_TRUE(frontier.push_if_new("http://test.com"));
    EXPECT_FALSE(frontier.push_if_new("http://test.com")); // duplicate
    
    EXPECT_EQ(frontier.size(), 1);
    
    auto url = frontier.pop_wait();
    EXPECT_TRUE(url.has_value());
    EXPECT_EQ(url.value(), "http://test.com");
    EXPECT_EQ(frontier.size(), 0);
}

TEST(URLFrontierTest, MultiThreadShutdown) {
    loom::URLFrontier frontier(100);
    
    std::thread consumer([&]() {
        auto optional_url = frontier.pop_wait();
        EXPECT_FALSE(optional_url.has_value());
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    frontier.shutdown();
    consumer.join();
}
