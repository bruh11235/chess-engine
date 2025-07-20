#include "MinimaxBot.h"
#include "utils.h"
#include <bits/stdc++.h>
using namespace std;

constexpr int MAX_DEPTH = 5;
constexpr int INF = 999999;

int nodes_visited;  // For debugging/logging purpose only

// [[from, to, promote], score]
pair<tuple<int, int, int>, int> MinimaxBot::minimax_search(
    const Color self_turn, const Color current_turn, const int depth) {

    nodes_visited++;

    // Max depth reached
    if (depth == MAX_DEPTH) {
        return {{}, state_eval(engine.get_state(), self_turn)};
    }

    // Game ended
    const vector<tuple<int, int, int>> moves = engine.get_legal_moves();
    if (moves.empty()) {
        if (game_end(engine) == 1) return {{}, self_turn == current_turn ? -INF + depth : INF - depth};
        return {};
    }

    // Recursive search
    vector<tuple<int, int, int>> best_moves;
    int best_score = (self_turn == current_turn ? -INF : INF);
    for (auto [from, to, promote] : moves) {
        engine.move(from, to, promote);
        const Color next_turn = current_turn == Color::White ? Color::Black : Color::White;
        auto [move, score] = minimax_search(self_turn, next_turn, depth + 1);
        engine.unmove();

        if ((current_turn == self_turn && score > best_score) || (current_turn != self_turn && score < best_score)) {
            best_score = score;
            best_moves.clear();
        }
        if (score == best_score) {
            best_moves.emplace_back(from, to, promote);
        }
    }

    uniform_int_distribution<> random_index(0, static_cast<int>(best_moves.size() - 1));
    return {best_moves[random_index(rng)], best_score};
}


tuple<int, int, int> MinimaxBot::bestmove(string command) {
    nodes_visited = 0;
    auto [move, score] = minimax_search(engine.get_turn(), engine.get_turn(), 0);
    cout << "info string Minimax score: " << score << "\n";
    cout << "info string Minimax nodes: " << nodes_visited << "\n";
    return move;
}
