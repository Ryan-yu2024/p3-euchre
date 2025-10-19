#include "Card.hpp"
#include "Pack.hpp"
#include "Player.hpp"
#include <iostream>
#include <string>
#include <cassert>

using namespace std;

void play_hand(Pack &pack, Player *players[], int &dealer, int scores[], bool shuffle);
void deal_cards(Pack &pack, Player *players[], int dealer);
int make_trump(Player *players[], const Card &upcard, int dealer, Suit &trump, bool &alone);
int play_tricks(Player *players[], int leader, Suit trump);
void score_hand(int &team0, int &team1, int maker_team, int tricks_won);


int main(int argc, char *argv[]) {

    if (argc != 12) {
        cout << "Usage: euchre.exe PACK_FILENAME [shuffle|noshuffle] POINTS_TO_WIN "
                "NAME1 TYPE1 NAME2 TYPE2 NAME3 TYPE3 NAME4 TYPE4" << endl;
        return 1;
    }

    string pack_filename = argv[1];
    string shuffle_opt = argv[2];
    bool shuffle = (shuffle_opt == "shuffle");
    int points_to_win = stoi(argv[3]);

    // player setup
    Player *players[4];
    for (int i = 0; i < 4; ++i) {
        string name = argv[4 + 2 * i];
        string type = argv[5 + 2 * i];
        players[i] = Player_factory(name, type);
    }

    // pack setup
    Pack pack(pack_filename);

    int scores[2] = {0, 0};   
    int dealer = 0;

    // main loop
    while (scores[0] < points_to_win && scores[1] < points_to_win) {
        play_hand(pack, players, dealer, scores, shuffle);
        dealer = (dealer + 1) % 4;
        cout << endl;
    }

    // printing
    if (scores[0] >= points_to_win)
        cout << players[0]->get_name() << " and " << players[2]->get_name() << " win!" << endl;
    else
        cout << players[1]->get_name() << " and " << players[3]->get_name() << " win!" << endl;

    for (int i = 0; i < 4; ++i)
        delete players[i];

    return 0;
}

// play full hand
void play_hand(Pack &pack, Player *players[], int &dealer, int scores[], bool shuffle) {
    cout << players[dealer]->get_name() << "deals" << endl;

    if (shuffle) pack.shuffle();
    else pack.reset();

    // deal
    deal_cards(pack, players, dealer);

    // turn up card
    Card upcard = pack.deal_one();
    cout << upcard << " turned up" << endl;

    // make trump
    Suit trump;
    bool alone = false;
    int maker = make_trump(players, upcard, dealer, trump, alone);
    int maker_team = maker % 2; 

    cout << players[maker]->get_name() << " orders up " << trump << endl;

    // play 5 tricks
    int leader = (dealer + 1) % 4;
    int tricks_team0 = 0;
    int tricks_team1 = 0;

    for (int i = 0; i < 5; ++i) {
        int winner = play_tricks(players, leader, trump);
        cout << players[winner]->get_name() << " takes the trick" << endl;
        if (winner % 2 == 0) ++tricks_team0;
        else ++tricks_team1;
        leader = winner;
    }

    // score the hand
    score_hand(scores[0], scores[1], maker_team, 
               maker_team == 0 ? tricks_team0 : tricks_team1);

    cout << players[0]->get_name() << " and " << players[2]->get_name() 
         << " have " << scores[0] << " points" << endl;
    cout << players[1]->get_name() << " and " << players[3]->get_name() 
         << " have " << scores[1] << " points" << endl;
}

// helper functions
void deal_cards(Pack &pack, Player *players[], int dealer) {
    // 3-2-3-2 or 2-3-2-3
    int order[4];
    for(int i = 0; i < 4; ++i) {
        order[i] = (dealer + 1 + i) %4;
    }

    for (int round = 0; round < 2; ++round) {
        for (int i = 0; i < 4; ++i) {
            int num_cards = ((i + round) % 2 == 0) ? 3 : 2;
            for (int c = 0; c < num_cards; ++c) {
                players[order[i]]->add_card(pack.deal_one());
            }
        }
    }

}

int make_trump(Player *players[], const Card &upcard, int dealer, Suit &trump, bool &alone) {
    // two rounds
    int order[4];
    for(int i = 0; i < 4; ++i) {
        order[i] = (dealer + 1 + i) %4;
    }

    // round 1
    for (int i = 1; i <= 4; ++i) {
        int idx = (dealer + i) % 4;
        Suit order_up_suit;
        bool is_dealer = (idx == dealer);
    }
        if (players[idx]->make_trump(upcard, is_dealer, 1, order_up_suit)) {
            maker = idx;
            trump = order_up_suit;

            if (is_dealer) {
                players[idx]->add_and_discard(upcard);

            }
            return maker;
        }
    }

    // round 2
    for (int i = 1; i <= 4; ++i) {
        int idx = (dealer + i) % 4;
        Suit order_up_suit;
        bool is_dealer = (idx == dealer);

        if (players[idx]->make_trump(upcard, is_dealer, 2, order_up_suit)) {
            maker = idx;
            trump = order_up_suit;
            return maker;
        }
    }

    // return index of maker
    return (dealer + 1) % 4;

int play_tricks(Player *players[], int leader, Suit trump) {

    // return winner
    return leader;
}

void score_hand(int &team0, int &team1, int maker_team, int tricks_won) {
    // update scores 

    // if maker_team 3+ tricks won = 1 point
    if (tricks_won >= 3 && tricks_won < 5) {
        if (maker_team == 0)
            team0 += 1;
        else
            team1 += 1;
        }
        // if 5 tricks won = 2 points
        else if (tricks_won == 5) {
            if (maker_team == 0)
            team0 += 2;
        else
            team1 += 2;
        }
        // if < 3, other team = 2
        else if (tricks_won < 3) {
             if (maker_team == 0)
            team1 += 2;
        else
            team0 += 2;
        }
    }
