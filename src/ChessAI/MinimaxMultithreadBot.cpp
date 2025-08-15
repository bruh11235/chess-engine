#include "MinimaxMultithreadBot.h"
#include "utils.h"
#include <bits/stdc++.h>
using namespace std;

constexpr int MAX_DEPTH = 6;
constexpr int NO_THREAD = 6;
constexpr int INF = 999999;

atomic<int> nodes_visited;


void MinimaxMultithreadBot::order_moves(vector<tuple<int, int, int>> &moves, const Color self_turn, const bool ascending) const {
    vector<pair<int, tuple<int, int, int>>> move_score;
    for (tuple<int, int, int> move : moves) {
        auto [from, to, promote] = move;
        engine.move(from, to, promote);
        const int score = state_eval(engine.get_state(), self_turn);
        engine.unmove();
        move_score.emplace_back(score, move);
    }

    if (ascending) {
        ranges::sort(move_score);
    } else {
        ranges::sort(move_score, greater<>());
    }

    for (size_t i = 0; i < moves.size(); i++) {
        moves[i] = move_score[i].second;
    }
}


// [[from, to, promote], score]
pair<tuple<int, int, int>, int> MinimaxMultithreadBot::minimax_thread(
    const Color self_turn, const Color current_turn, const int depth,
    int alpha, int beta, vector<tuple<int, int, int>> moves) {

    nodes_visited += 1;

    // Max depth reached
    uniform_int_distribution<> noise(-1, 1);
    if (depth == MAX_DEPTH) {
        return {{}, state_eval(engine.get_state(), self_turn) + noise(rng)};
    }

    // Game ended
    if (moves.empty()) {
        moves = engine.get_legal_moves();
    }
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
            auto [move, score] = minimax_thread(self_turn, next_turn, depth + 1, alpha, beta);
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
            auto [move, score] = minimax_thread(self_turn, next_turn, depth + 1, alpha, beta);
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


// Young Brother Wait
// Returns: [[from, to, promote], score]
pair<tuple<int, int, int>, int> MinimaxMultithreadBot::minimax_YBW(const Color self_turn) {
    vector<tuple<int, int, int>> moves = engine.get_legal_moves();
    pair<int, tuple<int, int, int>> best_result;
    int alpha;

    thread threads[NO_THREAD];
    vector<tuple<int, int, int>> thread_moves[NO_THREAD];
    pair<int, tuple<int, int, int>> thread_score[NO_THREAD];

    order_moves(moves, self_turn, false);
    for (int i = 0; i < moves.size(); i++) {
        auto [from, to, promote] = moves[i];

        // Search first child serially
        if (i == 0) {
            engine.move(from, to, promote);
            const Color next_turn = self_turn == Color::White ? Color::Black : Color::White;
            auto [move, score] = minimax_thread(self_turn, next_turn, 1, -INF, INF);
            best_result = {score, move};
            alpha = score;
            engine.unmove();
        }

        // Paralize other children
        else {
            thread_moves[i % NO_THREAD].push_back(moves[i]);
        }
    }

    // Run threads
    for (int i = 0; i < NO_THREAD; i++) {
        const Color next_turn = self_turn == Color::White ? Color::Black : Color::White;
        threads[i] = thread([&thread_score, &next_turn, &thread_moves, &i, &alpha, &self_turn, this] {
            auto [fst, snd] = minimax_thread(
                self_turn, next_turn, 1, alpha, INF, thread_moves[i]);
            thread_score[i] = {snd, fst};
        });
    }

    // Join threads
    for (int i = 0; i < NO_THREAD; i++) {
        threads[i].join();
        best_result = max(best_result, thread_score[i]);
    }


    return {best_result.second, best_result.first};
}


tuple<int, int, int> MinimaxMultithreadBot::bestmove(const string command) {
    assert(command.substr(0, 2) == "go");
    nodes_visited = 0;
    auto [move, score] = minimax_YBW(engine.get_turn());
    cout << "info string Minimax score: " << score << "\n";
    cout << "info string Minimax nodes: " << nodes_visited << "\n";
    cout << "info string Zobrist: " << engine.get_hash() << "\n";
    return move;
}
