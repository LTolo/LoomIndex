#include <gtest/gtest.h>
#include "LoomIndex/BloomFilter.hpp"
#include <string>

TEST(BloomFilterTest, BasicFunctionality) {
    loom::BloomFilter bf(1000, 0.01);
    
    EXPECT_FALSE(bf.possibly_contains("https://example.com"));
    
    bf.add("https://example.com");
    EXPECT_TRUE(bf.possibly_contains("https://example.com"));
    EXPECT_FALSE(bf.possibly_contains("https://google.com"));
}

TEST(BloomFilterTest, ZeroExpectedElements) {
    loom::BloomFilter bf(0, 0.01);
    bf.add("test");
    EXPECT_TRUE(bf.possibly_contains("test"));
}

TEST(BloomFilterTest, FalsePositiveRate) {
    loom::BloomFilter bf(10000, 0.05); // 5% expected FPR
    
    for (int i = 0; i < 10000; ++i) {
        bf.add("item_" + std::to_string(i));
    }
    
    int false_positives = 0;
    for (int i = 10000; i < 20000; ++i) {
        if (bf.possibly_contains("item_" + std::to_string(i))) {
            false_positives++;
        }
    }
    
    double actual_fpr = static_cast<double>(false_positives) / 10000.0;
    
    // allow some variance (5% target -> should be < 8%)
    EXPECT_LT(actual_fpr, 0.08);
}
