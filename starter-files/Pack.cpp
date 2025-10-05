#include "Pack.hpp"
#include <algorithm>
#include <cassert>
#include <istream>
using namespace std;

Pack::Pack()
    :cards{}, next(0) {
    static const Rank RANKS[] = {NINE, TEN, JACK, QUEEN, KING, ACE};
    static const Suit SUITS[] = {SPADES, HEARTS, CLUBS, DIAMONDS};

    int i = 0;
    for (Suit s: SUITS) {
        for (Rank r: RANKS) {
            cards[i] = Card(r, s);
            i++;
        }
    }
}

Pack::Pack(std::istream &pack_input) : cards{}, next(0) {
    for (int i = 0; i < PACK_SIZE; i++) {
        pack_input >> cards[i];
    }
}

Card Pack::deal_one() {
    assert(next < PACK_SIZE);
    Card c = cards[next];
    next++;
    return c;
}

void Pack::reset() {
    next = 0;
}

void Pack::shuffle() {
    int half = PACK_SIZE / 2;
    for(int shuff = 0; shuff < 7; shuff++) {
        array<Card, PACK_SIZE> old = cards;
        int out = 0;
        for (int i = 0; i < half; i++) {
            cards[out++] = old[half+i];
            cards[out++] = old[i];
        }

    }
    next = 0;
}

bool Pack::empty() const {
    return next >= PACK_SIZE;
}