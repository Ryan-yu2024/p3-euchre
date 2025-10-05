#include "Pack.hpp"
#include "unit_test_framework.hpp"

#include <iostream>

using namespace std;

TEST(test_pack_default_ctor) {
    Pack pack;
    Card first = pack.deal_one();
    ASSERT_EQUAL(NINE, first.get_rank());
    ASSERT_EQUAL(SPADES, first.get_suit());
}

// Add more tests here
TEST(test_dealing) {
    Pack pack;
    Card one = pack.deal_one();
    Card two = pack.deal_one();
    ASSERT_NOT_EQUAL(one, two);
}

TEST(test_dealing_reset) {
    Pack pack;
    Card one = pack.deal_one();
    pack.reset();
    Card two = pack.deal_one();
    ASSERT_NOT_EQUAL(one, two);
}

TEST(test_empty_pack) {
    Pack pack;
    for (int i = 0; i < 24; i++) {
        pack.deal_one();
    }
    ASSERT_TRUE(pack.empty())
}

TEST(test_shuffle) {
    Pack pack;
    Card one = pack.deal_one();
    pack.reset();
    pack.shuffle();
    Card two = pack.deal_one();
    ASSERT_NOT_EQUAL(one, two);
}

TEST(test_shuffle_full_cycle) {
    Pack original;
    Pack shuffled_pack;
    for (int i = 0; i < 23; i++) {
        shuffled_pack.shuffle();
    }
    ASSERT_EQUAL(original.deal_one(), shuffled_pack.deal_one());
}
TEST_MAIN()
