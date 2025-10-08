#include "Player.hpp"
#include "Card.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
using namespace std;

class Simple : public Player {
public:
    Simple(const string &name_in);
    const string & get_name() const override;
    void add_card(const Card &c) override;
    bool make_trump(const Card &upcard, bool is_dealer,
        int round, Suit &order_up_suit) const override;
    void add_and_discard(const Card &upcard) override;
    Card lead_card(Suit trump) override;
    Card play_card(const Card &led_card, Suit trump) override;
private:
    string name;
    vector<Card> hand;
};

Simple::Simple(const string &name_in) : name(name_in) {
}

const string & Simple::get_name() const {
    return name;
}

void Simple::add_card(const Card &c) {
    assert(hand.size() < MAX_HAND_SIZE);
    hand.push_back(c);
}

bool Simple::make_trump(const Card &upcard, bool is_dealer, 
                        int round, Suit &order_up_suit) const {
    assert(round == 1 || round == 2);

    if (round == 1) {
        int same_suit = 0;
        for (size_t i = 0; i < hand.size(); ++i) {
            const Card &card = hand[i];
            // Round 1: printed suit only
            if (card.get_suit() == upcard.get_suit() && card.is_face_or_ace()) {
                ++same_suit;
            }
        }
        if (same_suit >= 2) {
            order_up_suit = upcard.get_suit();
            return true;
        }
        return false;
    } else { // round == 2
        Suit next_suit = Suit_next(upcard.get_suit());
        int next = 0;
        for (size_t i = 0; i < hand.size(); ++i) {
            const Card &card = hand[i];
            // Round 2: evaluate as-if next_suit were trump
            if (card.get_suit(next_suit) == next_suit && card.is_face_or_ace()) {
                ++next;
            }
        }
        if (next >= 1 || is_dealer) { // stick-the-dealer
            order_up_suit = next_suit;
            return true;
        }
        return false;
    }
}

void Simple::add_and_discard(const Card &upcard) {
  assert(hand.size() >= 1);
  hand.push_back(upcard);
  int low = 0;
    Suit trump = upcard.get_suit();
    for (int i = 0; i < hand.size(); i++) {
        if (Card_less(hand[i], hand[low], trump)) {
            low = static_cast<int>(i);
        }
    }
    hand.erase(hand.begin() + low);
}


Card Simple::lead_card(Suit trump) {
    assert(hand.size() >= 1);
    int best_nontrump = -1;
    for (size_t i = 0; i < hand.size(); ++i) {
        if (!hand[i].is_trump(trump)) {
            if (best_nontrump == -1 || !Card_less(hand[i], hand[best_nontrump], trump)) {
                best_nontrump = static_cast<int>(i);
            }
        }
    }
    if (best_nontrump != -1) {
        Card out = hand[best_nontrump];
        hand.erase(hand.begin() + best_nontrump);
        return out;
    }
    int best = 0;
    for (size_t i = 1; i < hand.size(); ++i) {
        if (!Card_less(hand[i], hand[best], trump)) best = static_cast<int>(i);
    }
    Card out = hand[best];
    hand.erase(hand.begin() + best);
    return out;
}


Card Simple::play_card(const Card &led_card, Suit trump) {
    assert(hand.size() >= 1); 
    vector<Card> follow;
    Suit wanted_suit = led_card.get_suit(trump);

    for (size_t i = 0; i < hand.size(); ++i) {
        if (hand[i].get_suit(trump) == wanted_suit) {
            follow.push_back(hand[i]);
        }
    }

    if (!follow.empty()) {
        // play highest of the led suit
        int best = 0;
        for (size_t i = 1; i < follow.size(); ++i) {
            if (!Card_less(follow[i], follow[best], trump)) best = static_cast<int>(i);
        }
        Card chosen = follow[best];
        // remove chosen from hand
        for (size_t i = 0; i < hand.size(); ++i) {
            if (hand[i] == chosen) {
                hand.erase(hand.begin() + static_cast<int>(i));
                break;
            }
        }
        return chosen;
    } else {
        // cannot follow suit: play lowest card
        int worst = 0;
        for (size_t i = 1; i < hand.size(); ++i) {
            if (Card_less(hand[i], hand[worst], trump)) worst = static_cast<int>(i);
        }
        Card chosen = hand[worst];
        hand.erase(hand.begin() + worst);
        return chosen;
    }
}

class Human : public Player {
public:
    Human(const std::string &name_in) : name(name_in) {}
    const std::string & get_name() const override { return name;}

   void add_card(const Card &c) override {
        hand.push_back(c);
    }

    bool make_trump(const Card &, bool, int, Suit &) const override {
        return false;
    }

    void add_and_discard(const Card &c) override {
        hand.push_back(c);
        hand.pop_back();
    }

    Card lead_card(Suit) override {
        Card c = hand.back();
        hand.pop_back();
        return c;
    }

    Card play_card(const Card &, Suit) override {
        Card c = hand.back();
        hand.pop_back();
        return c;
    }

private:
    std:: string name;
    std::vector<Card> hand;
};

Player * Player_factory(const std::string &name, const std::string &strategy) {
    if (strategy == "Simple") {
        return new Simple(name);
    }
    else if (strategy == "Human") {
        return new Human(name);
    }
    else {
        assert(false);
        return nullptr;
    }
}

std::ostream & operator<<(std::ostream &os, const Player &p) {
    os << p.get_name();
    return os;
}
