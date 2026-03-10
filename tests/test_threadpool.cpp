#include <gtest/gtest.h>
#include "LoomIndex/ThreadPool.hpp"
#include <atomic>
#include <thread>
#include <chrono>

TEST(ThreadPoolTest, CounterValidationRAII) {
    std::atomic<int> counter{0};
    {
        loom::ThreadPool pool(4);
        for (int i = 0; i < 100; ++i) {
            pool.enqueue([&counter]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
                counter++;
            });
        }
    } // destructor blocks until all tasks are finished
    EXPECT_EQ(counter.load(), 100);
}

TEST(ThreadPoolTest, SequentialExecutionTest) {
    loom::ThreadPool pool(2);
    std::atomic<int> counter{0};
    
    for (int i = 0; i < 50; ++i) {
        pool.enqueue([&counter]() {
            counter++;
        });
    }
    
    // Give some time for threads to empty queue
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    EXPECT_EQ(counter.load(), 50);
}
