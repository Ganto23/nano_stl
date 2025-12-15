#include <gtest/gtest.h>
#include <nstl/vector.hpp>

// Test 1: Does it behave like a vector?
TEST(VectorTest, PushBackAndAccess) {
    nstl::vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);

    // EXPECT_EQ doesn't stop the test if it fails. ASSERT_EQ does.
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], 10);
    EXPECT_EQ(v[2], 30);
}

// Test 2: Does pop_back work and check bounds?
TEST(VectorTest, PopBackLogic) {
    nstl::vector<int> v;
    v.push_back(100);
    v.pop_back();

    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);

    // This verifies your "throw std::out_of_range" logic works!
    EXPECT_THROW(v.pop_back(), std::out_of_range);
}

// Test 3: The "HFT" Resize Logic
// We want to ensure capacity grows geometrically (doubling)
TEST(VectorTest, CapacityGrowth) {
    nstl::vector<int> v;
    size_t old_cap = v.capacity();
    
    // Force a few resizes
    for(int i=0; i < 100; ++i) {
        v.push_back(i);
        if (v.capacity() > old_cap) {
            // Verify it grew by at least 2x (or from 0 to 1)
            EXPECT_GE(v.capacity(), old_cap * 2); 
            old_cap = v.capacity();
        }
    }
}

// Test 4: Copy Constructor (Deep Copy)
TEST(VectorTest, DeepCopy) {
    nstl::vector<int> v1;
    v1.push_back(5);
    
    nstl::vector<int> v2 = v1; // Copy constructor
    v2[0] = 10; // Modify v2

    EXPECT_EQ(v1[0], 5);  // v1 should NOT change
    EXPECT_EQ(v2[0], 10); // v2 SHOULD change
}
