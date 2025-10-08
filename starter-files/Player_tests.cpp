#include "Player.hpp"
#include "unit_test_framework.hpp"

#include <iostream>

using namespace std;

TEST(test_player_get_name) {
    Player * alice = Player_factory("Alice", "Simple");
    ASSERT_EQUAL("Alice", alice->get_name());
    delete alice;
}

TEST(test_add_card) {
    Player *ryan = Player_factory("Ryan", "Simple");
    ryan->add_card(Card(KING, SPADES));
    ryan->add_card(Card(QUEEN, HEARTS));
    ryan->add_card(Card(TEN, HEARTS));
    Card c = ryan->lead_card(DIAMONDS);
    bool card_exists = (c == Card(KING, SPADES) || c == Card(QUEEN, HEARTS) || c == Card(TEN, HEARTS));
    ASSERT_TRUE(card_exists);
    delete ryan;
}

TEST(test_lead_card_trump) {
    Player *marley = Player_factory("Marley", "Simple");
    marley->add_card(Card(TEN, HEARTS));
    marley->add_card(Card(NINE, HEARTS));
    marley->add_card(Card(ACE, HEARTS));
    marley->add_card(Card(QUEEN, HEARTS));
    marley->add_card(Card(KING, HEARTS));

    Card leader = marley->lead_card(HEARTS);
    ASSERT_EQUAL(leader, Card(ACE, HEARTS));
    delete marley;
}

TEST(test_lead_card_not_trump) {
    Player *marley = Player_factory("Marley", "Simple");
    marley->add_card(Card(TEN, HEARTS));
    marley->add_card(Card(NINE, HEARTS));
    marley->add_card(Card(ACE, HEARTS));
    marley->add_card(Card(QUEEN, HEARTS));
    marley->add_card(Card(KING, HEARTS));

    Card leader = marley->lead_card(SPADES);
    ASSERT_EQUAL(leader, Card(ACE, HEARTS));    
    delete marley;
}

TEST(test_play_card_trump) {
    Player *squidward = Player_factory("Squidward", "Simple");
    squidward->add_card(Card(TEN, HEARTS));
    squidward->add_card(Card(NINE, HEARTS));
    squidward->add_card(Card(ACE, HEARTS));
    squidward->add_card(Card(QUEEN, HEARTS));
    squidward->add_card(Card(KING, HEARTS));
    Card c(JACK, HEARTS);

    Card played = squidward->play_card(c, HEARTS);
    ASSERT_EQUAL(played, Card(ACE, HEARTS)); 
    delete squidward;
}

TEST(test_play_card_non_trump) {
    Player *squidward = Player_factory("Squidward", "Simple");
    squidward->add_card(Card(TEN, HEARTS));
    squidward->add_card(Card(NINE, HEARTS));
    squidward->add_card(Card(ACE, HEARTS));
    squidward->add_card(Card(QUEEN, HEARTS));
    squidward->add_card(Card(KING, HEARTS));
    Card c(JACK, SPADES);

    Card played = squidward->play_card(c, HEARTS);
    ASSERT_EQUAL(played, Card(NINE, HEARTS)); 
    delete squidward;
}

TEST(test_make_trump_1) {
    Player *timmy = Player_factory("Timmy", "Simple");
    timmy->add_card(Card(NINE, HEARTS));
    timmy->add_card(Card(TEN, CLUBS));
    timmy->add_card(Card(KING, DIAMONDS));
    timmy->add_card(Card(JACK, HEARTS));
    timmy->add_card(Card(KING, SPADES));

    Suit order = CLUBS;
    bool choice = timmy->make_trump(Card(NINE, HEARTS), false, 1, order);
    ASSERT_FALSE(choice);
    ASSERT_EQUAL(order, CLUBS);
    delete timmy;
}

TEST(test_make_trump_2) {
    Player *timmy = Player_factory("Timmy", "Simple");
    timmy->add_card(Card(NINE, HEARTS));
    timmy->add_card(Card(TEN, CLUBS));
    timmy->add_card(Card(KING, DIAMONDS));
    timmy->add_card(Card(JACK, DIAMONDS));
    timmy->add_card(Card(KING, SPADES));

    Suit order = CLUBS;
    bool choice = timmy->make_trump(Card(NINE, HEARTS), false, 1, order);
    ASSERT_FALSE(choice);
    ASSERT_EQUAL(order, CLUBS);
    delete timmy;
}

TEST(test_make_trump_3) {
    Player *jimmy = Player_factory("Jimmy", "Simple");
    jimmy->add_card(Card(NINE, HEARTS));
    jimmy->add_card(Card(TEN, CLUBS));
    jimmy->add_card(Card(KING, DIAMONDS));
    jimmy->add_card(Card(JACK, DIAMONDS));
    jimmy->add_card(Card(KING, SPADES));

    Suit order = CLUBS;
    bool choice = jimmy->make_trump(Card(NINE, HEARTS), true, 2, order);
    ASSERT_TRUE(choice);
    ASSERT_EQUAL(order, DIAMONDS);   
    delete jimmy;
}

TEST(test_make_trump_4) {
    Player *kimmy = Player_factory("Kimmy", "Simple");
    kimmy->add_card(Card(NINE, HEARTS));
    kimmy->add_card(Card(TEN, CLUBS));
    kimmy->add_card(Card(KING, DIAMONDS));
    kimmy->add_card(Card(JACK, DIAMONDS));
    kimmy->add_card(Card(KING, DIAMONDS));

    Suit order = CLUBS;
    bool choice = kimmy->make_trump(Card(NINE, HEARTS), false, 2, order);
    ASSERT_TRUE(choice);
    ASSERT_EQUAL(order, DIAMONDS);   
    delete kimmy;
}

TEST(test_make_trump_1_pass) {
    Player *spongebob = Player_factory("Spongebob", "Simple");
    spongebob->add_card(Card(TEN, CLUBS));
    spongebob->add_card(Card(NINE, DIAMONDS));
    spongebob->add_card(Card(JACK, HEARTS));
    spongebob->add_card(Card(QUEEN, SPADES));
    spongebob->add_card(Card(KING, SPADES));

    Suit order = CLUBS;
    bool choice = spongebob->make_trump(Card(JACK, HEARTS), false, 1, order);
    ASSERT_FALSE(choice);
    ASSERT_EQUAL(order, CLUBS);
    delete spongebob;
}

TEST(test_make_trump_2_pass) {
    Player *patrick = Player_factory("Patrick", "Simple");
    patrick->add_card(Card(NINE, HEARTS));
    patrick->add_card(Card(TEN, CLUBS));
    patrick->add_card(Card(KING, SPADES));
    patrick->add_card(Card(QUEEN, SPADES));
    patrick->add_card(Card(JACK, CLUBS));

    Suit order = SPADES;
    bool choice = patrick->make_trump(Card(NINE, HEARTS), false, 2, order);
    ASSERT_FALSE(choice);
    ASSERT_EQUAL(order, SPADES);
    delete patrick;
}

TEST(test_add_and_discard) {
    Player *gary = Player_factory("Gary", "Simple");
    gary->add_card(Card(TEN, HEARTS));
    gary->add_card(Card(NINE, HEARTS));
    gary->add_card(Card(QUEEN, HEARTS));
    gary->add_card(Card(KING, HEARTS));
    gary->add_card(Card(JACK, HEARTS));

    gary->add_and_discard(Card(JACK, SPADES));

    Card c(NINE, HEARTS);
    bool still_has_lowest = false;

    for (int i = 0; i < 5; i++) {
        Card lead = gary->lead_card(SPADES);
        if (lead == c) {
            still_has_lowest = true;
        }
    }

    ASSERT_FALSE(still_has_lowest);
    delete gary;
}

TEST_MAIN()
