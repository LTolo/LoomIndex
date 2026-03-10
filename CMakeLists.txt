cmake_minimum_required(VERSION 3.20)
project(LoomIndex VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

# Find libcurl
find_package(CURL REQUIRED)

# Fetch GTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Include directories
include_directories(include)

# Main Library Target
add_library(LoomIndexLib
    src/Logger.cpp
    src/BloomFilter.cpp 
    src/ThreadPool.cpp
    src/URLFrontier.cpp 
    src/AsyncFetcher.cpp 
    src/CrawlerEngine.cpp
)
# Make library an interface for now until we have source files, or just compile dummy
# Let's use target_include_directories
target_include_directories(LoomIndexLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(LoomIndexLib PUBLIC CURL::libcurl)

# Executable (Optional CLI)
add_executable(LoomIndex src/main.cpp)
target_link_libraries(LoomIndex PRIVATE LoomIndexLib)

# Add Tests Directory
enable_testing()
add_subdirectory(tests)
