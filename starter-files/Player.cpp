// Player.cpp
#include "Player.hpp"
#include "Card.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Simple player.
class Simple : public Player {
public:
  explicit Simple(const string &name_in) : name(name_in) {}

  const string & get_name() const override { return name; }

  void add_card(const Card &c) override {
    assert(hand.size() < MAX_HAND_SIZE);
    hand.push_back(c);
  }

  bool make_trump(const Card &upcard, bool is_dealer,
                  int round, Suit &order_up_suit) const override {
    assert(round == 1 || round == 2);

    if (round == 1) {
      // Count face-or-ace *trumps* relative to upcard.suit
      Suit s = upcard.get_suit();
      int count = 0;
      for (const auto &c : hand) {
        if (c.is_trump(s) && c.is_face_or_ace()) ++count;
      }
      if (count >= 2) {
        order_up_suit = s;
        return true;
      }
      return false;
    } else {
      Suit s = Suit_next(upcard.get_suit());
      int count = 0;
      for (const auto &c : hand) {
        if (c.get_suit(s) == s && c.is_face_or_ace()) ++count;
      }
      if (count >= 1) {
        order_up_suit = s;
        return true;
      }
      // Screw the dealer
      if (is_dealer) {
        order_up_suit = s;
        return true;
      }
      return false;
    }
  }

  void add_and_discard(const Card &upcard) override {
    assert(hand.size() >= 1);
    hand.push_back(upcard);
    Suit trump = upcard.get_suit();
    size_t low = 0;
    for (size_t i = 1; i < hand.size(); ++i) {
      if (Card_less(hand[i], hand[low], trump)) low = i;
    }
    hand.erase(hand.begin() + static_cast<long>(low));
  }

  Card lead_card(Suit trump) override {
    assert(!hand.empty());
    int best_nontrump = -1;
    for (size_t i = 0; i < hand.size(); ++i) {
      if (!hand[i].is_trump(trump)) {
        if (best_nontrump == -1 ||
            !Card_less(hand[i], hand[best_nontrump], trump)) {
          best_nontrump = static_cast<int>(i);
        }
      }
    }
    int idx = -1;
    if (best_nontrump != -1) {
      idx = best_nontrump;
    } else {
      idx = 0;
      for (size_t i = 1; i < hand.size(); ++i) {
        if (!Card_less(hand[i], hand[idx], trump))
          idx = static_cast<int>(i);
      }
    }
    Card out = hand[static_cast<size_t>(idx)];
    hand.erase(hand.begin() + idx);
    return out;
  }

  Card play_card(const Card &led_card, Suit trump) override {
    assert(!hand.empty());
   
    Suit led_suit = led_card.get_suit(trump);
     vector<size_t> can_follow;
     vector<size_t> trump_cards;
    for (size_t i = 0; i < hand.size(); ++i) {
      if (hand[i].get_suit(trump) == led_suit) can_follow.push_back(i);
    }

    size_t idx = 0;
    if (!can_follow.empty()) {
      idx = can_follow[0];
      for (size_t k = 1; k < can_follow.size(); ++k) {
        size_t i = can_follow[k];
        if (!Card_less(hand[i], hand[idx], trump)) idx = i;
      }
    } else {
      for (size_t i = 1; i < hand.size(); ++i) {
        if (Card_less(hand[i], hand[idx], trump)) idx = i;
      }
    }

    Card out = hand[idx];
    hand.erase(hand.begin() + static_cast<long>(idx));
    return out;
  }

private:
  string name;
  vector<Card> hand;
};

//Human Player

class Human : public Player {
public:
  explicit Human(const string &name_in) : name(name_in) {}

  const string & get_name() const override { return name; }

  void add_card(const Card &c) override {
    hand.push_back(c);
    sort(hand.begin(), hand.end()); // ascending
  }

  bool make_trump(const Card &upcard, bool is_dealer,
                  int round, Suit &order_up_suit) const override {
    (void)upcard; (void)is_dealer;
    print_hand();
    cout << "Human player " << name
         << ", please enter a suit, or \"pass\":\n";

    string decision;
    cin >> decision;

    if (decision == "pass") {
      return false;
    }
    order_up_suit = string_to_suit(decision);
    return true;
  }

  void add_and_discard(const Card &upcard) override {
    // Show ONLY the current 5-card hand
    print_hand();
    cout << "Discard upcard: [-1]\n";
    cout << "Human player " << name
         << ", please select a card to discard:\n";
  
    int choice = 0;
    cin >> choice;
  
    // Hand is assumed valid
    if (choice != -1) {
      assert(choice >= 0 && static_cast<size_t>(choice) < hand.size());
      // Replace the chosen card with the upcard
      hand[static_cast<size_t>(choice)] = upcard;
      std::sort(hand.begin(), hand.end()); // keep invariant
    }
  }
  

  Card lead_card(Suit trump) override {
    (void)trump; // unused parameter
    print_hand();
    cout << "Human player " << name << ", please select a card:\n";
    int choice = 0;
    cin >> choice;
    assert(choice >= 0 && static_cast<size_t>(choice) < hand.size());
    Card out = hand[static_cast<size_t>(choice)];
    hand.erase(hand.begin() + choice);
    return out;
  }
  
  Card play_card(const Card &led_card, Suit trump) override {
    (void)led_card; // unused parameter
    (void)trump;
    print_hand();
    cout << "Human player " << name << ", please select a card:\n";
    int choice = 0;
    cin >> choice;
    assert(choice >= 0 && static_cast<size_t>(choice) < hand.size());
    Card out = hand[static_cast<size_t>(choice)];
    hand.erase(hand.begin() + choice);
    return out;
  }
  

private:
  void print_hand() const {
    // Hand must be printed in ascending order as defined by operator<
    // We keep it sorted on every mutation, so just print:
    for (size_t i = 0; i < hand.size(); ++i) {
      cout << "Human player " << name << "'s hand: "
           << "[" << i << "] " << hand[i] << "\n";
    }
  }

  string name;
  vector<Card> hand; //keep sorted by ascending
};

// Player factory. 

Player * Player_factory(const std::string &name, const std::string &strategy) {
  if (strategy == "Simple") return new Simple(name);
  if (strategy == "Human")  return new Human(name);
  assert(false);
  return nullptr;
}

std::ostream & operator<<(std::ostream &os, const Player &p) {
  os << p.get_name();
  return os;
}
