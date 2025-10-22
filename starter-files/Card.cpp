// Card.cpp
#include <cassert>
#include <iostream>
#include <array>
#include <string>
#include "Card.hpp"

using namespace std;

// Rank helpers (given interface)

constexpr const char *const RANK_NAMES[] = {
  "Two", "Three", "Four", "Five", "Six", "Seven",
  "Eight", "Nine", "Ten", "Jack", "Queen", "King", "Ace"
};

Rank string_to_rank(const std::string &str) {
  for (int r = TWO; r <= ACE; ++r) {
    if (str == RANK_NAMES[r]) {
      return static_cast<Rank>(r);
    }
  }
  assert(false);
  return {};
}

std::ostream & operator<<(std::ostream &os, Rank rank) {
  os << RANK_NAMES[rank];
  return os;
}

std::istream & operator>>(std::istream &is, Rank &rank) {
  string str;
  if (is >> str) {
    rank = string_to_rank(str);
  }
  return is;
}

// Suit helpers (given interface)

constexpr const char *const SUIT_NAMES[] = {
  "Spades", "Hearts", "Clubs", "Diamonds",
};

Suit string_to_suit(const std::string &str) {
  for (int s = SPADES; s <= DIAMONDS; ++s) {
    if (str == SUIT_NAMES[s]) {
      return static_cast<Suit>(s);
    }
  }
  assert(false);
  return {};
}

std::ostream & operator<<(std::ostream &os, Suit suit) {
  os << SUIT_NAMES[suit];
  return os;
}

std::istream & operator>>(std::istream &is, Suit &suit) {
  string str;
  if (is >> str) {
    suit = string_to_suit(str);
  }
  return is;
}

// Card implementation
Card::Card() : rank(TWO), suit(SPADES) {}

Card::Card(Rank rank_in, Suit suit_in)
  : rank(rank_in), suit(suit_in) {}

Rank Card::get_rank() const { return rank; }
Suit Card::get_suit() const { return suit; }

// Left bower counts as trump suit.
Suit Card::get_suit(Suit trump) const {
  if (is_left_bower(trump)) {
    return trump;
  }
  return suit;
}

bool Card::is_face_or_ace() const {
  return rank == JACK || rank == QUEEN || rank == KING || rank == ACE;
}

bool Card::is_right_bower(Suit trump) const {
  return rank == JACK && suit == trump;
}

bool Card::is_left_bower(Suit trump) const {
  return rank == JACK && suit == Suit_next(trump);
}

bool Card::is_trump(Suit trump) const {
  return get_suit(trump) == trump;
}

// Stream ops & relops 
ostream &operator<<(ostream &os, const Card &card) {
  os << card.get_rank() << " of " << card.get_suit();
  return os;
}

istream &operator>>(istream &is, Card &card) {
  Rank r;
  string of_word;
  Suit s;

  if (!(is >> r)) return is;
  if (!(is >> of_word)) return is;
  if (of_word != "of") { is.setstate(std::ios::failbit); return is; }
  if (!(is >> s)) return is;

  card = Card(r, s);
  return is;
}

// Natural order (no trump/led): rank, then suit tie-break.
bool operator<(const Card &lhs, const Card &rhs) {
  if (lhs.get_rank() != rhs.get_rank()) {
    return static_cast<int>(lhs.get_rank()) <
           static_cast<int>(rhs.get_rank());
  }
  return static_cast<int>(lhs.get_suit()) <
         static_cast<int>(rhs.get_suit());
}

bool operator>(const Card &l, const Card &r)  { return r < l; }
bool operator<=(const Card &l, const Card &r) { return !(r < l); }
bool operator>=(const Card &l, const Card &r) { return !(l < r); }

bool operator==(const Card &l, const Card &r) {
  return l.get_rank() == r.get_rank() && l.get_suit() == r.get_suit();
}
bool operator!=(const Card &l, const Card &r) { return !(l == r); }

// Helpers

// Same-color mapping for left bower: H <-> D, C <-> S
Suit Suit_next(Suit s) {
  if (s == HEARTS)   return DIAMONDS;
  if (s == DIAMONDS) return HEARTS;
  if (s == CLUBS)    return SPADES;
  if (s == SPADES)   return CLUBS;
  return SPADES;  // never reached
}

// Factorized bower comparison to avoid duplication in Card_less.
static int compare_bowers(const Card &a, const Card &b, Suit trump) {
  // +1 if a > b due to bower, -1 if a < b, 0 if neither determines
  if (a.is_right_bower(trump)) return b.is_right_bower(trump) ? 0 : +1;
  if (b.is_right_bower(trump)) return -1;
  if (a.is_left_bower(trump))  return b.is_left_bower(trump)  ? 0 : +1;
  if (b.is_left_bower(trump))  return -1;
  return 0;
}

// Contextual comparisons
bool Card_less(const Card &a, const Card &b, Suit trump) {
  if (int cmp = compare_bowers(a, b, trump)) return cmp < 0;

  const bool a_trump = a.is_trump(trump);
  const bool b_trump = b.is_trump(trump);

  // Any trump beats any non-trump.
  if (a_trump != b_trump) return !a_trump && b_trump;

  // Both trump or both non-trump: rank, then suit tie-break.
  int ra = static_cast<int>(a.get_rank());
  int rb = static_cast<int>(b.get_rank());
  if (ra != rb) return ra < rb;
  return a.get_suit() < b.get_suit();
}

bool Card_less(const Card &a, 
               const Card &b,
               const Card &led_card, 
               Suit trump) {
  const Suit led_suit = led_card.get_suit(trump);

  if (int cmp = compare_bowers(a, b, trump)) return cmp < 0;

  const bool a_trump = a.is_trump(trump);
  const bool b_trump = b.is_trump(trump);

  if (a_trump != b_trump) return !a_trump && b_trump;

  if (a_trump && b_trump) {
    int ra = static_cast<int>(a.get_rank());
    int rb = static_cast<int>(b.get_rank());
    if (ra != rb) return ra < rb;
    return a.get_suit() < b.get_suit();
  }

  // Neither trump: following led suit beats not following.
  const bool a_follows = (a.get_suit(trump) == led_suit);
  const bool b_follows = (b.get_suit(trump) == led_suit);
  if (a_follows != b_follows) return !a_follows && b_follows;

  int ra = static_cast<int>(a.get_rank());
  int rb = static_cast<int>(b.get_rank());
  if (ra != rb) return ra < rb;
  return a.get_suit() < b.get_suit();
}
