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
        for (int i = 0; i < hand.size(); i++) {
            const Card &card = hand[i];
            if (card.get_suit() == upcard.get_suit() &&
        card.is_face_or_ace()) {
            same_suit++;
        }
        }
        if (same_suit >= 2) {
            order_up_suit = upcard.get_suit();
            return true;
        }
        return false;
    }
    else if (round == 2) {
        Suit next_suit = Suit_next(upcard.get_suit());
        int next = 0;
        for (int i = 0; i < hand.size(); i++) {
            const Card &card = hand[i];
            if (card.get_suit() == next_suit && card.is_face_or_ace()) {
                next++;
            }
        }
        if (next >= 1 || is_dealer) {
            order_up_suit = next_suit;
            return true;
        }
        return false;
    }
    return false;
}

void Simple::add_and_discard(const Card &upcard) {
    assert(hand.size() >= 1);
    hand.push_back(upcard);
    int low = 0;
    Suit trump = upcard.get_suit();
    for (int i = 0; i < hand.size(); i++) {
        if (Card_less(hand[i], hand[low], trump)) {
            low = i;
        }
    }
    hand.erase(hand.begin() + low);
}

Card Simple::lead_card(Suit trump) {
    assert(hand.size() >= 1);
    int best = -1;
    for (int i = 0; i < hand.size(); i++) {
        if (hand[i].is_trump(trump) == false) {
            if (best == -1 || Card_less(hand[i], hand[best], trump) == false) {
                best = i;
        }
        
        }
    }
    if (best > -1) {
        Card temp = hand[best];
        hand.erase(hand.begin() + best);
        return temp;
    }
    else if (best == -1) {
        int greatest = 0;
        for (int i = 0; i < hand.size(); i++) {
            if (Card_less(hand[i], hand[greatest], trump) == false) {
                greatest = i;
            }
        }
        Card temp = hand[greatest];
        hand.erase(hand.begin() + greatest);
        return temp;
    }
}

Card Simple::play_card(const Card &led_card, Suit trump) {
    assert(hand.size() >= 1); 
    vector<Card> v;
    Suit wanted_suit = led_card.get_suit(trump);
    for (int i = 0; i < hand.size(); i++) {
        if (hand[i].get_suit(trump) == wanted_suit) {
            v.push_back(hand[i]);
        }
    }
    if (v.empty() == false) {
        int best = 0;
        for (int i = 0; i < v.size(); i++) {
            if (Card_less(v[i], v[best], trump) == false) {
                best = i;
            }
        }
        Card temp = v[best];
        for (int i = 0; i < hand.size(); i++) {
            if (hand[i] == temp) {
                hand.erase(hand.begin() + i);
                break;
            }
        }
        return temp;
        
    }
    else {
        int worst = 0;
        for (int i = 0; i < hand.size(); i++) {
            if (Card_less(hand[i], hand[worst], trump)) {
                worst = i;
            }
        }
        Card temp = hand[worst];
        hand.erase(hand.begin() + worst);
        return temp;
    }
};

Player * Player_factory(const std::string &name, const std::string &strategy) {
    if (strategy == "Simple") {
        return new Simple(name);
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
