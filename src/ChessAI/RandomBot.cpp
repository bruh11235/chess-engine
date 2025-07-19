#include "RandomBot.h"
#include <bits/stdc++.h>
using namespace std;

tuple<int, int, int> RandomBot::bestmove(string command) {
    const vector<tuple<int, int, int>> moves = engine.get_legal_moves();
    uniform_int_distribution dist(0, static_cast<int>(moves.size()) - 1);
    return moves[dist(rng)];
}
