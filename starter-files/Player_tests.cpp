// Player Tests
#include "Player.hpp"
#include "unit_test_framework.hpp"

#include <iostream>

using namespace std;

// PLAYER BASIC FUNCTIONALITY ----

TEST(test_player_get_name) {
    Player* alice = Player_factory("Alice", "Simple");
    ASSERT_EQUAL("Alice", alice->get_name());
    delete alice;
}

TEST(test_add_card) {
    Player* ryan = Player_factory("Ryan", "Simple");
    ryan->add_card(Card(KING, SPADES));
    ryan->add_card(Card(QUEEN, HEARTS));
    ryan->add_card(Card(TEN, HEARTS));
    Card c = ryan->lead_card(DIAMONDS);
    bool card_exists = (c == Card(KING, SPADES) || c == Card(QUEEN, HEARTS) || c == Card(TEN, HEARTS));
    ASSERT_TRUE(card_exists);
    delete ryan;
}

// LEAD CARD LOGIC ----

TEST(test_lead_card_trump) {
    Player* marley = Player_factory("Marley", "Simple");
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
    Player* marley = Player_factory("Marley", "Simple");
    marley->add_card(Card(TEN, HEARTS));
    marley->add_card(Card(NINE, HEARTS));
    marley->add_card(Card(ACE, HEARTS));
    marley->add_card(Card(QUEEN, HEARTS));
    marley->add_card(Card(KING, HEARTS));

    Card leader = marley->lead_card(SPADES);
    ASSERT_EQUAL(leader, Card(ACE, HEARTS));
    delete marley;
}

TEST(test_lead_card_non_trump) {
    Player* max = Player_factory("Max", "Simple");
    max->add_card(Card(NINE, HEARTS));
    max->add_card(Card(ACE, CLUBS));
    max->add_card(Card(TEN, SPADES));
    max->add_card(Card(KING, CLUBS));
    max->add_card(Card(QUEEN, CLUBS));

    Card lead = max->lead_card(HEARTS);
    ASSERT_EQUAL(lead, Card(ACE, CLUBS));
    delete max;
}

TEST(test_lead_card_chooses_highest_non_trump_when_available) {
    Player* karen = Player_factory("Karen", "Simple");
    karen->add_card(Card(ACE, SPADES));
    karen->add_card(Card(KING, SPADES));
    karen->add_card(Card(QUEEN, HEARTS));
    karen->add_card(Card(TEN, HEARTS));
    karen->add_card(Card(NINE, CLUBS));

    Card led = karen->lead_card(HEARTS);
    ASSERT_EQUAL(led, Card(ACE, SPADES));
    delete karen;
}

TEST(test_lead_card_all_trump_leads_strongest_trump) {
    Player* pearl = Player_factory("Pearl", "Simple");
    pearl->add_card(Card(NINE, DIAMONDS));
    pearl->add_card(Card(TEN, DIAMONDS));
    pearl->add_card(Card(QUEEN, DIAMONDS));
    pearl->add_card(Card(KING, DIAMONDS));
    pearl->add_card(Card(JACK, HEARTS));

    Card led = pearl->lead_card(DIAMONDS);
    ASSERT_EQUAL(led, Card(JACK, HEARTS));
    delete pearl;
}

// PLAY CARD LOGIC ---

TEST(test_play_card_non_trump) {
    Player* squidward = Player_factory("Squidward", "Simple");
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

TEST(test_play_card_left_bower_follow) {
    Player* andrew = Player_factory("Andrew", "Simple");
    andrew->add_card(Card(JACK, DIAMONDS));
    andrew->add_card(Card(JACK, HEARTS));
    andrew->add_card(Card(ACE, SPADES));
    andrew->add_card(Card(TEN, CLUBS));
    andrew->add_card(Card(KING, HEARTS));

    Card led(TEN, HEARTS);
    Card played = andrew->play_card(led, HEARTS);
    ASSERT_EQUAL(played, Card(JACK, HEARTS));
    delete andrew;
}

TEST(test_play_card_left_bower) {
    Player* larry = Player_factory("Larry", "Simple");
    larry->add_card(Card(NINE, HEARTS));
    larry->add_card(Card(ACE, CLUBS));
    larry->add_card(Card(TEN, SPADES));
    larry->add_card(Card(KING, DIAMONDS));
    larry->add_card(Card(QUEEN, CLUBS));

    Card led(JACK, DIAMONDS);
    Card play = larry->play_card(led, HEARTS);
    ASSERT_EQUAL(play, Card(NINE, HEARTS));
    delete larry;
}

TEST(test_play_card_must_follow) {
    Player* puff = Player_factory("Puff", "Simple");
    puff->add_card(Card(NINE, SPADES));
    puff->add_card(Card(QUEEN, SPADES));
    puff->add_card(Card(KING, SPADES));
    puff->add_card(Card(TEN, CLUBS));
    puff->add_card(Card(NINE, HEARTS));

    Card led(TEN, SPADES);
    Card played = puff->play_card(led, HEARTS);
    ASSERT_EQUAL(played, Card(KING, SPADES));
    delete puff;
}

TEST(test_play_card_cannot_follow) {
    Player* gary = Player_factory("Gary", "Simple");
    gary->add_card(Card(NINE, CLUBS));
    gary->add_card(Card(TEN, DIAMONDS));
    gary->add_card(Card(QUEEN, CLUBS));
    gary->add_card(Card(TEN, SPADES));
    gary->add_card(Card(ACE, SPADES));

    Card led(KING, HEARTS);
    Card played = gary->play_card(led, SPADES);
    ASSERT_EQUAL(played, Card(NINE, CLUBS));
    delete gary;
}

TEST(test_play_card_one_in_hand) {
    Player* goofy = Player_factory("Goofy", "Simple");
    goofy->add_card(Card(ACE, SPADES));

    Card led(TEN, HEARTS);
    Card played = goofy->play_card(led, HEARTS);
    ASSERT_EQUAL(played, Card(ACE, SPADES));
    delete goofy;
}

// MAKE TRUMP LOGIC ---

TEST(test_make_trump_round1_pass) {
    Player* spongebob = Player_factory("Spongebob", "Simple");
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

TEST(test_make_trump_round2_order) {
    Player* plankton = Player_factory("Plankton", "Simple");
    plankton->add_card(Card(JACK, HEARTS));
    plankton->add_card(Card(NINE, SPADES));
    plankton->add_card(Card(TEN, CLUBS));
    plankton->add_card(Card(KING, SPADES));
    plankton->add_card(Card(NINE, DIAMONDS));

    Suit order = CLUBS;
    bool choice = plankton->make_trump(Card(NINE, HEARTS), false, 2, order);
    ASSERT_TRUE(choice);
    ASSERT_EQUAL(order, DIAMONDS);
    delete plankton;
}

TEST(test_make_trump_screw_dealer_round2) {
    Player* pluto = Player_factory("Pluto", "Simple");
    pluto->add_card(Card(NINE, HEARTS));
    pluto->add_card(Card(TEN, CLUBS));
    pluto->add_card(Card(NINE, DIAMONDS));
    pluto->add_card(Card(TEN, DIAMONDS));
    pluto->add_card(Card(NINE, SPADES));

    Suit order = SPADES;
    bool choice = pluto->make_trump(Card(NINE, HEARTS), true, 2, order);
    ASSERT_TRUE(choice); // dealer must order up
    ASSERT_EQUAL(order, DIAMONDS); // next suit of HEARTS
    delete pluto;
}

// ADD AND DISCARD LOGIC ---

TEST(test_add_and_discard) {
    Player* gary = Player_factory("Gary", "Simple");
    gary->add_card(Card(TEN, HEARTS));
    gary->add_card(Card(NINE, HEARTS));
    gary->add_card(Card(QUEEN, HEARTS));
    gary->add_card(Card(KING, HEARTS));
    gary->add_card(Card(JACK, HEARTS));

    gary->add_and_discard(Card(ACE, HEARTS));
    Card leader = gary->lead_card(HEARTS);
    ASSERT_EQUAL(leader, Card(JACK, HEARTS));
    delete gary;
}

// CARD UTILITY / BOWER LOGIC ---

TEST(test_card_less_bowers) {
    Suit trump = HEARTS;
    Card right_bower(JACK, HEARTS);
    Card left_bower(JACK, DIAMONDS);
    Card ace_hearts(ACE, HEARTS);

    ASSERT_TRUE(Card_less(left_bower, right_bower, trump));
    ASSERT_TRUE(Card_less(ace_hearts, right_bower, trump));
    ASSERT_TRUE(Card_less(ace_hearts, left_bower, trump));
}

TEST(test_card_bower_mapping) {
    Card jack_hearts(JACK, HEARTS);
    Card jack_diamonds(JACK, DIAMONDS);
    Card jack_clubs(JACK, CLUBS);
    Card jack_spades(JACK, SPADES);

    ASSERT_TRUE(jack_hearts.is_right_bower(HEARTS));
    ASSERT_TRUE(jack_diamonds.is_left_bower(HEARTS));
    ASSERT_TRUE(jack_diamonds.is_trump(HEARTS));
    ASSERT_TRUE(jack_clubs.is_right_bower(CLUBS));
    ASSERT_TRUE(jack_spades.is_left_bower(CLUBS));
    ASSERT_TRUE(jack_spades.is_trump(CLUBS));
}

TEST(test_card_get_suit_maps_left_bower) {
    Card jd(JACK, DIAMONDS);
    ASSERT_EQUAL(jd.get_suit(HEARTS), HEARTS);
    ASSERT_TRUE(jd.is_trump(HEARTS));
    ASSERT_FALSE(jd.is_right_bower(HEARTS));
    ASSERT_TRUE(jd.is_left_bower(HEARTS));
}

TEST_MAIN()
