// euchre.cpp
// Driver Program playing full game
#include "Card.hpp"
#include "Pack.hpp"
#include "Player.hpp"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::vector;

//Usage for euchre.cpp.
static void usage_and_exit() {
  cout << "Usage: euchre.exe PACK_FILENAME [shuffle|noshuffle] "
       << "POINTS_TO_WIN NAME1 TYPE1 NAME2 TYPE2 NAME3 TYPE3 "
       << "NAME4 TYPE4" << endl;
  std::exit(1);
}

// Returns true if t is valid player type
static bool is_player_type(const string &t) {
  return t == "Simple" || t == "Human";
}

// Returns formatted team names for players
static string team02_names(Player *p0, Player *p2) {
  return p0->get_name() + string(" and ") + p2->get_name();
}
static string team13_names(Player *p1, Player *p3) {
  return p1->get_name() + string(" and ") + p3->get_name();
}

// Deal 3-2-3-2 then 2-3-2-3, starting left of dealer.
static void deal_hand(Pack &pack, vector<Player *> &players, int dealer_seat) {
  int seat = (dealer_seat + 1) % 4;

  // First pass: 3-2-3-2
  for (int i = 0; i < 4; ++i) {
    int num = (i % 2 == 0) ? 3 : 2;
    for (int j = 0; j < num; ++j) {
      players[seat]->add_card(pack.deal_one());
    }
    seat = (seat + 1) % 4;
  }

  // Second pass: 2-3-2-3
  seat = (dealer_seat + 1) % 4;
  for (int i = 0; i < 4; ++i) {
    int num = (i % 2 == 0) ? 2 : 3;
    for (int j = 0; j < num; ++j) {
      players[seat]->add_card(pack.deal_one());
    }
    seat = (seat + 1) % 4;
  }
}

//Use this struct in order to help play_trick take 4 parameters. 
struct TrickScore {
  int t02 = 0;
  int t13 = 0;
};

// Plays a single trick; prints, updates scores, returns winner seat.
static int play_trick(vector<Player *> &players,
                      int leader_seat,
                      Suit trump,
                      TrickScore &ts) {
  struct Play { int seat; Card card; };
  Play plays[4];

  plays[0] = {leader_seat, players[leader_seat]->lead_card(trump)};
  cout << plays[0].card << " led by " << *players[leader_seat] << endl;

  for (int step = 1; step < 4; ++step) {
    int pi = (leader_seat + step) % 4;
    plays[step] = {pi, players[pi]->play_card(plays[0].card, trump)};
    cout << plays[step].card << " played by " << *players[pi] << endl;
  }

  int winner = plays[0].seat;
  Card best = plays[0].card;
  for (int i = 1; i < 4; ++i) {
    if (Card_less(best, plays[i].card, plays[0].card, trump)) {
      best = plays[i].card;
      winner = plays[i].seat;
    }
  }

  cout << *players[winner] << " takes the trick" << endl;
  cout << endl;

  // Returns seat number of trick winner
  if (winner % 2 == 0) { ++ts.t02; } else { ++ts.t13; }
  return winner;
}

// Helper struct to limit parameters in trying round functions.
struct MakeCtx {
  bool ordered = false;
  int maker = -1;
  Suit trump = SPADES;
};

// Round 1: try ordering up the upcard suit
static void try_round_one(const Card &upcard,
                          int dealer,
                          vector<Player *> &P,
                          MakeCtx &mc) {
  for (int i = 1; i <= 4 && !mc.ordered; ++i) {
    int p = (dealer + i) % 4;
    Suit dummy;
    if (P[p]->make_trump(upcard, p == dealer, 1, dummy)) {
      mc.trump = upcard.get_suit();
      mc.maker = p;
      mc.ordered = true;
      cout << *P[p] << " orders up " << mc.trump << endl;
      // Dealer always picks up & discards on round 1 if anyone orders up
      P[dealer]->add_and_discard(upcard);
    } else {
      cout << *P[p] << " passes" << endl;
    }
  }
}

// Round 2: naming next suit
static void try_round_two(const Card &upcard,
                          int dealer,
                          vector<Player *> &P,
                          MakeCtx &mc) {
  if (mc.ordered) return;
  for (int i = 1; i <= 4 && !mc.ordered; ++i) {
    int p = (dealer + i) % 4;
    Suit chosen;
    if (P[p]->make_trump(upcard, p == dealer, 2, chosen)) {
      mc.trump = chosen;
      mc.maker = p;
      mc.ordered = true;
      cout << *P[p] << " orders up " << mc.trump << endl;
    } else {
      cout << *P[p] << " passes" << endl;
    }
  }
}

// Round 3: screw the dealer
static void screw_the_dealer(const Card &upcard,
                            int dealer,
                            MakeCtx &mc,
                            vector<Player *> &P) {
  if (mc.ordered) return;
  mc.maker = dealer;
  mc.trump = Suit_next(upcard.get_suit());
  mc.ordered = true;
  cout << *P[dealer] << " orders up " << mc.trump << endl;
}

// Main
int main(int argc, char *argv[]) {
  // Echo executable + args with a trailing space, then newline
  for (int i = 0; i < argc; ++i) {
    if (i) cout << ' ';
    cout << argv[i];
  }
  cout << ' ' << endl;

  if (argc != 12) {
    usage_and_exit();
  }

  const string pack_filename = argv[1];
  const string shuffle_flag  = argv[2];
  const string points_str    = argv[3];

  bool do_shuffle;
  if (shuffle_flag == "shuffle") {
    do_shuffle = true;
  } else if (shuffle_flag == "noshuffle") {
    do_shuffle = false;
  } else {
    usage_and_exit();
  }

  int points_to_win = 0;
  try {
    points_to_win = std::stoi(points_str);
  } catch (...) {
    usage_and_exit();
  }
  if (points_to_win < 1 || points_to_win > 100) usage_and_exit();

  const string name1 = argv[4],  type1 = argv[5];
  const string name2 = argv[6],  type2 = argv[7];
  const string name3 = argv[8],  type3 = argv[9];
  const string name4 = argv[10], type4 = argv[11];
  if (!is_player_type(type1) || !is_player_type(type2) ||
      !is_player_type(type3) || !is_player_type(type4)) {
    usage_and_exit();
  }

  // Open pack file; on error, print to stdout
  ifstream ifs(pack_filename);
  if (!ifs.is_open()) {
    cout << "Error opening " << pack_filename << endl;
    return 1;
  }
  Pack pack(ifs);

  // Create players
  vector<Player *> P(4, nullptr);
  P[0] = Player_factory(name1, type1);
  P[1] = Player_factory(name2, type2);
  P[2] = Player_factory(name3, type3);
  P[3] = Player_factory(name4, type4);

  const string team02 = team02_names(P[0], P[2]);
  const string team13 = team13_names(P[1], P[3]);

  int dealer = 0;
  int score02 = 0, score13 = 0;
  int hand_idx = 0;

  // Main game loop
  while (score02 < points_to_win && score13 < points_to_win) {
    if (hand_idx > 0) cout << endl; // blank line between hands

    cout << "Hand " << hand_idx << endl;
    cout << P[dealer]->get_name() << " deals" << endl;

    // Shuffle policy
    if (do_shuffle) {
      pack.shuffle();
    } else {
      pack.reset();
    }

    deal_hand(pack, P, dealer);

    Card upcard = pack.deal_one();
    cout << upcard << " turned up" << endl;

    // Make trump phases
    MakeCtx mc;
    try_round_one(upcard, dealer, P, mc);
    try_round_two(upcard, dealer, P, mc);
    screw_the_dealer(upcard, dealer, mc, P);

    // Extra blank line after make/discard completes
    cout << endl;

    // Play five tricks
    TrickScore ts;
    int leader = (dealer + 1) % 4;
    for (int trick = 0; trick < 5; ++trick) {
      leader = play_trick(P, leader, mc.trump, ts);
    }

    // Score the hand
    bool maker_is_team02 = (mc.maker % 2 == 0);
    int maker_tricks = maker_is_team02 ? ts.t02 : ts.t13;
    bool march = (maker_tricks == 5);
    bool euchred = (maker_tricks <= 2);

    if (march) {
      if (maker_is_team02) { score02 += 2; } else { score13 += 2; }
    } else if (euchred) {
      if (maker_is_team02) { score13 += 2; } else { score02 += 2; }
    } else {
      if (maker_is_team02) { ++score02; } else { ++score13; }
    }

    // Announce hand winner
    bool team02_won_hand = (ts.t02 > ts.t13);
    cout << (team02_won_hand ? team02 : team13) << " win the hand" << endl;
    if (march)   cout << "march!" << endl;
    if (euchred) cout << "euchred!" << endl;

    cout << team02 << " have " << score02 << " points" << endl;
    cout << team13 << " have " << score13 << " points" << endl;

    // End of game
    if (score02 >= points_to_win || score13 >= points_to_win) {
      cout << endl
           << (score02 >= points_to_win ? team02 : team13) << " win!" << endl;
      break;
    }

    dealer = (dealer + 1) % 4;
    ++hand_idx;
  }

  for (Player *p : P) delete p;
  return 0;
}
