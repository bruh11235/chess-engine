#include "GreedyBot.h"
#include "utils.h"
#include <bits/stdc++.h>
using namespace std;

tuple<int, int, int> GreedyBot::bestmove(string command) {
    vector<tuple<int, int, int>> moves = engine.get_legal_moves();
    const Color own_turn = engine.get_turn();
    vector<tuple<int, int, int>> best_moves;
    int best_score = -999999;

    for (auto [from, to, promote] : moves) {

        engine.move(from, to, promote);
        int score = state_eval(engine.get_state(), own_turn);
        if (game_end(engine) == 1) score = 999999;
        else if (game_end(engine) == 2) score = 0;
        engine.unmove();

        if (score > best_score) {
            best_score = score;
            best_moves.clear();
        }
        if (score == best_score) {
            best_moves.emplace_back(from, to, promote);
        }
    }

    uniform_int_distribution<> random_index(0, static_cast<int>(best_moves.size() - 1));
    return best_moves[random_index(rng)];
}
