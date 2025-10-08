#include "Card.hpp"
#include "unit_test_framework.hpp"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;


TEST(test_card_ctor) {
    Card c(ACE, HEARTS);
    ASSERT_EQUAL(ACE, c.get_rank());
    ASSERT_EQUAL(HEARTS, c.get_suit());
}

// Add more test cases here

TEST(test_operators) {
    Card a(ACE, CLUBS);
    Card b(ACE, SPADES);
    Card c(KING, DIAMONDS);

    ASSERT_TRUE(c < a);
    ASSERT_TRUE(!(a < c));
    ASSERT_TRUE(a != b);
    ASSERT_TRUE(a < b || b < a);
}

TEST(test_card_valid_inputs) {
    istringstream iss_bad("Ace Spades");
    Card c;
    iss_bad >> c;
    ASSERT_TRUE(iss_bad.fail());
}

TEST(test_rank_stream_roundtrip) {
    ostringstream oss;
    oss << QUEEN;
    ASSERT_EQUAL(string("Queen"), oss.str());

    istringstream iss("Nine");
    Rank r = TWO;
    iss >> r;
    ASSERT_EQUAL(NINE, r);
}

TEST(test_suit_stream_roundtrip) {
    ostringstream oss;
    oss << HEARTS;
    ASSERT_EQUAL(string("Hearts"), oss.str());

    istringstream iss("Diamonds");
    Suit s = SPADES;
    iss >> s;
    ASSERT_EQUAL(DIAMONDS, s);
}

TEST(test_Suit_next_all_cases) {
    ASSERT_EQUAL(CLUBS, Suit_next(SPADES));
    ASSERT_EQUAL(SPADES, Suit_next(CLUBS));
    ASSERT_EQUAL(DIAMONDS, Suit_next(HEARTS));
    ASSERT_EQUAL(HEARTS, Suit_next(DIAMONDS));
}

TEST(test_left_and_right_bower) {
    const Suit trump = SPADES;
    Card right(JACK, SPADES);
    Card left(JACK, CLUBS);

    ASSERT_TRUE(right.is_right_bower(trump));
    ASSERT_FALSE(right.is_left_bower(trump));

    ASSERT_TRUE(left.is_left_bower(trump));
    ASSERT_FALSE(left.is_right_bower(trump));

    ASSERT_EQUAL(trump, left.get_suit(trump));
    ASSERT_EQUAL(CLUBS, left.get_suit());
}

TEST(test_is_trump_including_left_bower) {
    const Suit trump = HEARTS;
    Card ace_hearts(ACE, HEARTS);
    Card jack_diamonds(JACK, DIAMONDS); 
    Card nine_spades(NINE, SPADES);

    ASSERT_TRUE(ace_hearts.is_trump(trump));
    ASSERT_TRUE(jack_diamonds.is_trump(trump)); 
    ASSERT_FALSE(nine_spades.is_trump(trump));
}

TEST(test_is_trump_including_bowers) {
    const Suit trump = HEARTS;

    Card ace_hearts(ACE, HEARTS);         
    Card jack_hearts(JACK, HEARTS);        
    Card jack_diamonds(JACK, DIAMONDS);  
    Card nine_spades(NINE, SPADES);  

    ASSERT_TRUE(ace_hearts.is_trump(trump));   
    ASSERT_TRUE(jack_hearts.is_trump(trump));    
    ASSERT_TRUE(jack_diamonds.is_trump(trump)); 
    ASSERT_FALSE(nine_spades.is_trump(trump));
}

TEST(test_card_less_trump_only) {
   const Suit trump = DIAMONDS;
    Card right(JACK, DIAMONDS);
    Card left(JACK, HEARTS);    
    Card ace_trump(ACE, DIAMONDS);
    Card ace_off(ACE, SPADES);

    ASSERT_TRUE(Card_less(left, right, trump));        
    ASSERT_TRUE(Card_less(ace_trump, left, trump));   
    ASSERT_TRUE(Card_less(ace_off, ace_trump, trump)); 

    Card nine_clubs(NINE, CLUBS);
    ASSERT_TRUE(Card_less(nine_clubs, ace_off, trump));
}

TEST(test_card_wit_led) {
    const Suit trump = CLUBS;
    Card led_card(QUEEN, DIAMONDS); 
    Card follower(TEN, DIAMONDS);
    Card offsuit(KING, HEARTS);
    Card trump_card(NINE, CLUBS);

    ASSERT_TRUE(Card_less(follower, trump_card, led_card, trump));
    ASSERT_TRUE(Card_less(offsuit, follower, led_card, trump));
    Card follower_hi(ACE, DIAMONDS);
    ASSERT_TRUE(Card_less(follower, follower_hi, led_card, trump));
}

TEST_MAIN()
