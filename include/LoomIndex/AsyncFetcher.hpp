#pragma once

#include <string>
#include <functional>
#include <memory>
#include <curl/curl.h>

namespace loom {

/**
 * @struct FetchResult
 * @brief Represents the result of an HTTP GET operation.
 */
struct FetchResult {
    long status_code;
    std::string url;
    std::string body;
    std::string error_message;
    bool success;
};

using FetchCallback = std::function<void(FetchResult)>;

/**
 * @class AsyncFetcher
 * @brief Asynchronous HTTP client based on libcurl (curl_multi).
 * Handles RAII cleanup of curl handles automatically.
 */
class AsyncFetcher {
public:
    AsyncFetcher();
    
    // RAII principle: cleanup curl multi and easy handles in destructor
    ~AsyncFetcher();

    AsyncFetcher(const AsyncFetcher&) = delete;
    AsyncFetcher& operator=(const AsyncFetcher&) = delete;

    /**
     * @brief Initiates an asynchronous fetch.
     * @param url Target URL to GET.
     * @param callback Called upon completion or failure.
     */
    void fetch_async(const std::string& url, FetchCallback callback);

    /**
     * @brief Pumps the curl_multi event loop. Should be called periodically or
     * integrated into an epoll/select event loop.
     */
    void perform_io();

private:
    // Raw pointers for libcurl handles, wrapped securely in this class
    CURLM* multi_handle_;
    
    // Conceptually we'd manage a mapping of CURL* to callbacks/context here.
    // E.g., structural wrappers for context:
    struct CurlContext {
        CURL* easy_handle;
        std::string url;
        std::string buffer;
        FetchCallback callback;
    };
    
    // Helper function to read data into our C++ string buffer
    static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata);
};

} // namespace loom
