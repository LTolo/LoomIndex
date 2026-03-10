#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

namespace loom {

/**
 * @class ThreadPool
 * @brief Manages a fixed number of worker threads.
 * Uses RAII for graceful shutdown.
 */
class ThreadPool {
public:
    /**
     * @brief Constructs the pool with a specified number of threads.
     * @param num_threads The number of worker threads to spawn.
     */
    explicit ThreadPool(std::size_t num_threads);

    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    /**
     * @brief Enqueues a task for execution by the pool.
     * @param task The task to execute.
     */
    void enqueue(std::function<void()> task);

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_;

    void worker_loop();
};

} // namespace loom
