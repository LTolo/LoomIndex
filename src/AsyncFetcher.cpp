#include "LoomIndex/AsyncFetcher.hpp"
#include "LoomIndex/Logger.hpp"

namespace loom {

// Global curl initialization should ideally be handled once per application lifecycle,
// but for simplicity in this component, we ensure it's not strictly required here if 
// curl_global_init is called in main(). It's safe to call curl_global_init multiple times 
// in standard libcurl if curl_global_cleanup matches.
// We assume caller handles global init/cleanup for performance in a real app.

AsyncFetcher::AsyncFetcher() {
    multi_handle_ = curl_multi_init();
    if (!multi_handle_) {
        throw std::runtime_error("Failed to initialize curl_multi handle");
    }
}

AsyncFetcher::~AsyncFetcher() {
    if (multi_handle_) {
        // Technically we should also clean up any remaining easy handles in the multi_handle.
        // For a robust implementation, we'd iterate over them here.
        curl_multi_cleanup(multi_handle_);
    }
}

size_t AsyncFetcher::write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    size_t real_size = size * nmemb;
    CurlContext* context = static_cast<CurlContext*>(userdata);
    context->buffer.append(ptr, real_size);
    return real_size;
}

void AsyncFetcher::fetch_async(const std::string& url, FetchCallback callback) {
    CURL* easy_handle = curl_easy_init();
    if (!easy_handle) {
        FetchResult result{0, url, "", "Failed to init curl easy handle", false};
        callback(std::move(result));
        return;
    }

    // Allocate context on the heap to persist across async event loop iterations
    CurlContext* context = new CurlContext{easy_handle, url, "", std::move(callback)};

    curl_easy_setopt(easy_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, context);
    curl_easy_setopt(easy_handle, CURLOPT_PRIVATE, context);
    
    // Follow redirects, set reasonable timeouts
    curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(easy_handle, CURLOPT_TIMEOUT, 10L); 
    curl_easy_setopt(easy_handle, CURLOPT_USERAGENT, "LoomIndexBot/0.1");

    curl_multi_add_handle(multi_handle_, easy_handle);
}

void AsyncFetcher::perform_io() {
    int still_running = 0;
    // Perform the curl state machine
    CURLMcode mc = curl_multi_perform(multi_handle_, &still_running);
    if (mc != CURLM_OK) {
        std::cerr << "curl_multi_perform failed: " << curl_multi_strerror(mc) << std::endl;
        return;
    }

    // Process finished transfers
    CURLMsg* msg;
    int msgs_left = 0;
    while ((msg = curl_multi_info_read(multi_handle_, &msgs_left))) {
        if (msg->msg == CURLMSG_DONE) {
            CURL* easy_handle = msg->easy_handle;
            CurlContext* context = nullptr;
            curl_easy_getinfo(easy_handle, CURLINFO_PRIVATE, &context);

            if (context) {
                FetchResult result;
                result.url = context->url;
                result.body = std::move(context->buffer);
                curl_easy_getinfo(easy_handle, CURLINFO_RESPONSE_CODE, &result.status_code);
                
                if (msg->data.result == CURLE_OK && result.status_code >= 200 && result.status_code < 400) {
                    result.success = true;
                } else {
                    result.success = false;
                    if (msg->data.result != CURLE_OK) {
                        result.error_message = curl_easy_strerror(msg->data.result);
                    } else {
                        result.error_message = "HTTP Error: " + std::to_string(result.status_code);
                    }
                    Logger::error("AsyncFetcher failed for URL [" + result.url + "]: " + result.error_message);
                }

                // Trigger callback safely (User is responsible for not throwing in callback)
                try {
                    context->callback(std::move(result));
                } catch (const std::exception& e) {
                    Logger::error("Exception in fetch callback for " + context->url + ": " + e.what());
                }

                // Cleanup
                curl_multi_remove_handle(multi_handle_, easy_handle);
                curl_easy_cleanup(easy_handle);
                delete context;
            }
        }
    }
}

} // namespace loom
