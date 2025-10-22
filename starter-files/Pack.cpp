// Pack.cpp
#include "Pack.hpp"
#include <algorithm>
#include <cassert>
#include <istream>
using namespace std;

// Initialize standard euchre deck
Pack::Pack() : cards{}, next(0) {
    static const Rank RANKS[] = {NINE, TEN, JACK, QUEEN, KING, ACE};
    static const Suit SUITS[] = {SPADES, HEARTS, CLUBS, DIAMONDS};

    int i = 0;
    for (Suit s : SUITS) {
        for (Rank r : RANKS) {
            cards[i++] = Card(r, s);
        }
    }
}

// Read 24 cards from input stream into deck
Pack::Pack(std::istream &pack_input) : cards{}, next(0) {
    for (int i = 0; i < PACK_SIZE; i++) {
        pack_input >> cards[i];
    }
}

Card Pack::deal_one() {
    assert(next < PACK_SIZE); // Ensure cards left to deal
    Card c = cards[next];
    next++;
    return c; // Return next card and increment index
}

void Pack::reset() {
    next = 0;
}

// 7 riffle shuffles
void Pack::shuffle() {
    for (int k = 0; k < 7; ++k) {
        std::array<Card, PACK_SIZE> old = cards;
        int index = 0;
        for (int i = 0; i < PACK_SIZE / 2; ++i) {
            cards[index++] = old[i + PACK_SIZE / 2];  // second half first
            cards[index++] = old[i];                  // first half next
        }
    }
    next = 0; // Reset index after shuffle
}

bool Pack::empty() const {
    return next >= PACK_SIZE;
}
