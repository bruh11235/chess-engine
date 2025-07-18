#include "RandomBot.h"
#include <bits/stdc++.h>
using namespace std;

RandomBot::RandomBot(ChessEngine &engine, const int seed) : AbstractBot(engine) {
    rng = mt19937(seed);
}


tuple<int, int, int> RandomBot::bestmove(string command) {
    bool is_legal = false;
    tuple<int, int, int> final_move;
    const vector<tuple<int, int, int>> moves = engine.get_moves();
    uniform_int_distribution dist(0, static_cast<int>(moves.size()) - 1);

    while (!is_legal) {
        is_legal = true;
        final_move = moves[dist(rng)];
        const auto [from, to, promote] = final_move;
        engine.move(from, to, promote);
        if (engine.has_check(engine.get_turn())) {
            is_legal = false;
        }
        engine.unmove();
    }
    return final_move;
}
