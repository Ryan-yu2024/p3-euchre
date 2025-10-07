#include <cassert>
#include <iostream>
#include <array>
#include "Card.hpp"

using namespace std;

/////////////// Rank operator implementations - DO NOT CHANGE ///////////////

constexpr const char *const RANK_NAMES[] = {
  "Two",   // TWO
  "Three", // THREE
  "Four",  // FOUR
  "Five",  // FIVE
  "Six",   // SIX
  "Seven", // SEVEN
  "Eight", // EIGHT
  "Nine",  // NINE
  "Ten",   // TEN
  "Jack",  // JACK
  "Queen", // QUEEN
  "King",  // KING
  "Ace"    // ACE
};

//REQUIRES str represents a valid rank ("Two", "Three", ..., "Ace")
//EFFECTS returns the Rank corresponding to str, for example "Two" -> TWO
Rank string_to_rank(const std::string &str) {
  for(int r = TWO; r <= ACE; ++r) {
    if (str == RANK_NAMES[r]) {
      return static_cast<Rank>(r);
    }
  }
  assert(false); // Input string didn't match any rank
  return {};
}

//EFFECTS Prints Rank to stream, for example "Two"
std::ostream & operator<<(std::ostream &os, Rank rank) {
  os << RANK_NAMES[rank];
  return os;
}

//REQUIRES If any input is read, it must be a valid rank
//EFFECTS Reads a Rank from a stream, for example "Two" -> TWO
std::istream & operator>>(std::istream &is, Rank &rank) {
  string str;
  if(is >> str) {
    rank = string_to_rank(str);
  }
  return is;
}



/////////////// Suit operator implementations - DO NOT CHANGE ///////////////

constexpr const char *const SUIT_NAMES[] = {
  "Spades",   // SPADES
  "Hearts",   // HEARTS
  "Clubs",    // CLUBS
  "Diamonds", // DIAMONDS
};

//REQUIRES str represents a valid suit ("Spades", "Hearts", "Clubs", or "Diamonds")
//EFFECTS returns the Suit corresponding to str, for example "Clubs" -> CLUBS
Suit string_to_suit(const std::string &str) {
  for(int s = SPADES; s <= DIAMONDS; ++s) {
    if (str == SUIT_NAMES[s]) {
      return static_cast<Suit>(s);
    }
  }
  assert(false); // Input string didn't match any suit
  return {};
}

//EFFECTS Prints Suit to stream, for example "Spades"
std::ostream & operator<<(std::ostream &os, Suit suit) {
  os << SUIT_NAMES[suit];
  return os;
}

//REQUIRES If any input is read, it must be a valid suit
//EFFECTS Reads a Suit from a stream, for example "Spades" -> SPADES
std::istream & operator>>(std::istream &is, Suit &suit) {
  string str;
  if (is >> str) {
    suit = string_to_suit(str);
  }
  return is;
}


/////////////// Write your implementation for Card below ///////////////
Card::Card() : rank(TWO), suit(SPADES) {}

Card::Card(Rank rank_in, Suit suit_in)
  : rank(rank_in), suit(suit_in) {}

Rank Card::get_rank() const {
  return rank;
}

Suit Card::get_suit() const {
  return suit;
}

Suit Card::get_suit(Suit trump) const {
  if (is_left_bower(trump)) {
    return trump;
  }
  return suit;
}

bool Card::is_face_or_ace() const{
  return rank == JACK || rank == QUEEN || rank == KING || rank == ACE;
}

bool Card::is_right_bower(Suit trump) const {
  return rank == JACK && suit == trump;
  
}
bool Card::is_left_bower(Suit trump) const {
  return rank == JACK && suit == Suit_next(trump);
}

bool Card::is_trump(Suit trump) const {
  return suit == trump || is_left_bower(trump);
}
// NOTE: We HIGHLY recommend you check out the operator overloading
// tutorial in the project spec before implementing
// the following operator overload functions:
//   operator<<
//   operator>>
//   operator<
//   operator<=
//   operator>
//   operator>=
//   operator==
//   operator!=

ostream &operator<<(ostream &os, const Card &card) {
  os << card.get_rank() << " of " << card.get_suit();
  return os;
}

istream &operator>>(istream &is, Card &card) {
  string next_word;
  Rank rank;
  Suit suit;
  is >> rank >> next_word >> suit;
  card = Card(rank, suit);
  return is;
}

bool operator<(const Card &left, const Card &right) {
  if (static_cast<int>(left.get_rank()) < static_cast<int>(right.get_rank())) {
    return true;
  } else if (static_cast<int>(left.get_rank()) > static_cast<int>(right.get_rank())) {
    return false;
  } else {
    return static_cast<int>(left.get_suit()) < static_cast<int>(right.get_suit());
  }
}

bool operator>(const Card &left, const Card&right) {
  return right < left;
}

bool operator<=(const Card &left, const Card&right) {
  return !(right < left);
}

bool operator>=(const Card &left, const Card&right) {
  return !(left < right);
}

bool operator==(const Card &left, const Card &right) {
  return left.get_rank() == right.get_rank() &&
         left.get_suit() == right.get_suit();
}

bool operator!=(const Card &left, const Card &right) {
  return !(left == right);
}

Suit Suit_next(Suit s) {
  if (s == HEARTS) {
    return DIAMONDS;
  }
  if (s == DIAMONDS) {
    return HEARTS;
  }
  if (s == CLUBS) {
    return SPADES;
  }
  if (s == SPADES) {
    return CLUBS;
  }
  return SPADES;
}

bool Card_less(const Card &a, const Card &b, Suit trump) {
  if (a.is_trump(trump) && !b.is_trump(trump)) {
    return false;
  }
  else if (!a.is_trump(trump) && b.is_trump(trump)) {
    return true;
  }
  else {
    return a < b;
  }
}

bool Card_less(const Card &a, const Card &b, const Card &led_card, Suit trump) {
  Suit led_suit = led_card.get_suit(trump);
  bool a_next = (a.get_suit(trump) == led_suit);
  bool b_next = (b.get_suit(trump) == led_suit);

  if (a.is_trump(trump) && !b.is_trump(trump)) {
    return false;
  }
  if (!a.is_trump(trump) && b.is_trump(trump)) {
    return true;
  }
  if (a_next && !b_next) {
    return false;
  }
  if (!a_next && b_next) {
    return true;
  }
  return a < b;
}