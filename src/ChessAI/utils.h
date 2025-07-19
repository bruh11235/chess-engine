#ifndef UTILS_H
#define UTILS_H

#include "../EngineCore/ChessEngine.h"


constexpr int PAWN_VALUE = 100;
constexpr int KNIGHT_VALUE = 300;
constexpr int BISHOP_VALUE = 300;
constexpr int ROOK_VALUE = 500;
constexpr int QUEEN_VALUE = 900;


inline int move_eval(const Bitboard &state, const Color &turn) {
    const bitboard_t own_pieces = (turn == Color::White ? state.white : state.black);
    const bitboard_t other_pieces = (turn == Color::White ? state.black : state.white);
    int score = 0;
    score += (std::popcount(own_pieces & state.pieces[0]) - std::popcount(other_pieces & state.pieces[0])) * PAWN_VALUE;
    score += (std::popcount(own_pieces & state.pieces[1]) - std::popcount(other_pieces & state.pieces[1])) * KNIGHT_VALUE;
    score += (std::popcount(own_pieces & state.pieces[2]) - std::popcount(other_pieces & state.pieces[2])) * BISHOP_VALUE;
    score += (std::popcount(own_pieces & state.pieces[3]) - std::popcount(other_pieces & state.pieces[3])) * ROOK_VALUE;
    score += (std::popcount(own_pieces & state.pieces[4]) - std::popcount(other_pieces & state.pieces[4])) * QUEEN_VALUE;
    return score;
}


// 0 = ongoing, 1 = lost, 2 = statemate
inline int game_end(ChessEngine &engine) {
    if (!engine.get_legal_moves().empty()) return 0;
    if (engine.has_check(engine.get_turn() == Color::White ? Color::Black : Color::White)) return 1;
    return 2;
}


#endif //UTILS_H
