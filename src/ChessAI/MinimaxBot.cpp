#include "MinimaxBot.h"
#include "utils.h"
#include <bits/stdc++.h>
using namespace std;

constexpr int MAX_DEPTH = 6;
constexpr int INF = 999999;

int nodes_visited;  // TODO: For debugging/logging purpose only
unordered_set<long long> unique_hashes;


void MinimaxBot::order_moves(vector<tuple<int, int, int>> &moves, const Color self_turn, bool ascending) const {
    ranges::sort(moves, [this, self_turn, ascending](const tuple<int, int, int> &a, const tuple<int, int, int> &b) {
        const auto [from_a, to_a, promote_a] = a;
        const auto [from_b, to_b, promote_b] = b;
        engine.move(from_a, to_a, promote_a);
        const int score_a = state_eval(engine.get_state(), self_turn);
        engine.unmove();
        engine.move(from_b, to_b, promote_b);
        const int score_b = state_eval(engine.get_state(), self_turn);
        engine.unmove();
        if (ascending) return score_a < score_b;
        return score_a > score_b;
    });
}


// [[from, to, promote], score]
pair<tuple<int, int, int>, int> MinimaxBot::minimax_search(
    const Color self_turn, const Color current_turn, const int depth, int alpha, int beta) {

    nodes_visited++;
    unique_hashes.insert(engine.get_hash());

    // Max depth reached
    uniform_int_distribution<> noise(-1, 1);
    if (depth == MAX_DEPTH) {
        return {{}, state_eval(engine.get_state(), self_turn) + noise(rng)};
    }

    // Game ended
    vector<tuple<int, int, int>> moves = engine.get_legal_moves();
    if (moves.empty()) {
        if (game_end(engine) == 1) return {{}, self_turn == current_turn ? -INF + depth * 5 : INF - depth * 5};
        return {};
    }

    // Recursive search
    tuple<int, int, int> best_move;
    if (self_turn == current_turn) {  // Maximizing
        order_moves(moves, self_turn, false);
        int best_score = -INF;
        for (auto [from, to, promote] : moves) {
            engine.move(from, to, promote);
            const Color next_turn = current_turn == Color::White ? Color::Black : Color::White;
            auto [move, score] = minimax_search(self_turn, next_turn, depth + 1, alpha, beta);
            engine.unmove();

            if (score > best_score) {
                best_score = score;
                best_move = {from, to, promote};
            }

            if (best_score >= beta) break;
            alpha = max(alpha, best_score);
        }

        return {best_move, best_score};
    } else {  // Minimizing
        order_moves(moves, self_turn, true);
        int best_score = INF;
        for (auto [from, to, promote] : moves) {
            engine.move(from, to, promote);
            const Color next_turn = current_turn == Color::White ? Color::Black : Color::White;
            auto [move, score] = minimax_search(self_turn, next_turn, depth + 1, alpha, beta);
            engine.unmove();

            if (score < best_score) {
                best_score = score;
                best_move = {from, to, promote};
            }

            if (best_score <= alpha) break;
            beta = min(beta, best_score);
        }

        return {best_move, best_score};
    }
}


tuple<int, int, int> MinimaxBot::bestmove(const string command) {
    assert(command.substr(0, 2) == "go");
    nodes_visited = 0;
    auto [move, score] = minimax_search(engine.get_turn(), engine.get_turn(), 0, -INF, INF);
    cout << "info string Minimax score: " << score << "\n";
    cout << "info string Minimax nodes: " << nodes_visited << " (" << unique_hashes.size() << " unique)\n";
    cout << "info string Zobrist: " << engine.get_hash() << "\n";
    return move;
}
