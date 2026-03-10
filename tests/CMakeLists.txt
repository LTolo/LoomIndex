cmake_minimum_required(VERSION 3.20)

add_executable(LoomIndexTests test_main.cpp test_bloom_filter.cpp test_threadpool.cpp test_url_frontier.cpp test_async_fetcher.cpp test_crawler_engine.cpp)

target_link_libraries(LoomIndexTests 
    PRIVATE 
    LoomIndexLib
    GTest::gtest_main 
    GTest::gtest
)

include(GoogleTest)
gtest_discover_tests(LoomIndexTests)
