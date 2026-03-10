#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <memory>
#include <functional>

namespace loom {

/**
 * @class BloomFilter
 * @brief O(k) lookup efficiency filter for fast duplicate URL detection.
 * Models RAII: Manages its internal bit array upon construction/destruction.
 */
class BloomFilter {
public:
    /**
     * @brief Constructs a Bloom filter with a defined optimal size and hash functions.
     * @param expected_elements Estimated number of elements to store.
     * @param false_positive_rate Desired probability of a false positive.
     */
    BloomFilter(std::size_t expected_elements, double false_positive_rate);

    ~BloomFilter() = default;

    // Prevent copying to ensure memory safety and avoid expensive deep copies
    BloomFilter(const BloomFilter&) = delete;
    BloomFilter& operator=(const BloomFilter&) = delete;

    // Allow moving
    BloomFilter(BloomFilter&&) noexcept = default;
    BloomFilter& operator=(BloomFilter&&) noexcept = default;

    /**
     * @brief Adds a URL to the filter.
     * @param url The string representation of the URL.
     */
    void add(const std::string& url);

    /**
     * @brief Checks if a URL *might* be in the filter.
     * @param url The string representation of the URL.
     * @return true if it might be contained, false if it is definitely NOT contained.
     */
    [[nodiscard]] bool possibly_contains(const std::string& url) const;

private:
    std::size_t num_bits_;
    std::size_t num_hashes_;
    std::vector<bool> bit_array_;

    // Internal helper to calculate multiple hashes for a given string
    [[nodiscard]] std::vector<std::size_t> hash(const std::string& item) const;
};

} // namespace loom
