#include "LoomIndex/BloomFilter.hpp"
#include <cmath>
#include <functional>

namespace loom {

BloomFilter::BloomFilter(std::size_t expected_elements, double false_positive_rate) {
    if (expected_elements == 0) {
        expected_elements = 1;
    }
    if (false_positive_rate <= 0.0 || false_positive_rate >= 1.0) {
        false_positive_rate = 0.01;
    }
    
    double m_double = - (static_cast<double>(expected_elements) * std::log(false_positive_rate)) / (std::pow(std::log(2.0), 2));
    num_bits_ = static_cast<std::size_t>(std::ceil(m_double));
    if (num_bits_ == 0) num_bits_ = 1;

    double k_double = (static_cast<double>(num_bits_) / static_cast<double>(expected_elements)) * std::log(2.0);
    num_hashes_ = static_cast<std::size_t>(std::ceil(k_double));
    if (num_hashes_ == 0) num_hashes_ = 1;

    bit_array_.resize(num_bits_, false);
}

std::vector<std::size_t> BloomFilter::hash(const std::string& item) const {
    std::vector<std::size_t> hashes;
    hashes.reserve(num_hashes_);
    
    std::size_t hash1 = std::hash<std::string>{}(item);
    
    std::size_t hash2 = 5381;
    for (char c : item) {
        hash2 = ((hash2 << 5) + hash2) + c; /* hash * 33 + c */
    }
    if (hash2 == 0) hash2 = 1;
    
    for (std::size_t i = 0; i < num_hashes_; ++i) {
        std::size_t h = hash1 + i * hash2;
        hashes.push_back(h % num_bits_);
    }
    
    return hashes;
}

void BloomFilter::add(const std::string& url) {
    std::vector<std::size_t> hashes = hash(url);
    for (std::size_t h : hashes) {
        bit_array_[h] = true;
    }
}

bool BloomFilter::possibly_contains(const std::string& url) const {
    std::vector<std::size_t> hashes = hash(url);
    for (std::size_t h : hashes) {
        if (!bit_array_[h]) {
            return false;
        }
    }
    return true;
}

} // namespace loom
