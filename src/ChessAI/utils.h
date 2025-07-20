#ifndef UTILS_H
#define UTILS_H

#include "../EngineCore/ChessEngine.h"


constexpr int PAWN_VALUE = 100;
constexpr int KNIGHT_VALUE = 320;
constexpr int BISHOP_VALUE = 330;
constexpr int ROOK_VALUE = 500;
constexpr int QUEEN_VALUE = 900;

constexpr int PAWN_PST[64] = {0,0,0,0,0,0,0,0,50,50,50,50,50,50,50,50,10,10,20,30,30,20,10,10,5,5,10,25,25,10,5,5,0,0,0,20,20,0,0,0,5,-5,-10,0,0,-10,-5,5,5,10,10,-20,-20,10,10,5,0,0,0,0,0,0,0,0};
constexpr int KNIGHT_PST[64] = {-50,-40,-30,-30,-30,-30,-40,-50,-40,-20,0,0,0,0,-20,-40,-30,0,10,15,15,10,0,-30,-30,5,15,20,20,15,5,-30,-30,0,15,20,20,15,0,-30,-30,5,10,15,15,10,5,-30,-40,-20,0,5,5,0,-20,-40,-50,-40,-30,-30,-30,-30,-40,-50};
constexpr int BISHOP_PST[64] = {-20,-10,-10,-10,-10,-10,-10,-20,-10,0,0,0,0,0,0,-10,-10,0,5,10,10,5,0,-10,-10,5,5,10,10,5,5,-10,-10,0,10,10,10,10,0,-10,-10,10,10,10,10,10,10,-10,-10,5,0,0,0,0,5,-10,-20,-10,-10,-10,-10,-10,-10,-20};
constexpr int ROOK_PST[64] = {0,0,0,0,0,0,0,0,5,10,10,10,10,10,10,5,-5,0,0,0,0,0,0,-5,-5,0,0,0,0,0,0,-5,-5,0,0,0,0,0,0,-5,-5,0,0,0,0,0,0,-5,-5,0,0,0,0,0,0,-5,0,0,0,5,5,0,0,0};
constexpr int QUEEN_PST[64] = {-20,-10,-10,-5,-5,-10,-10,-20,-10,0,0,0,0,0,0,-10,-10,0,5,5,5,5,0,-10,-5,0,5,5,5,5,0,-5,0,0,5,5,5,5,0,-5,-10,5,5,5,5,5,0,-10,-10,0,5,0,0,0,0,-10,-20,-10,-10,-5,-5,-10,-10,-20};
constexpr int KING_PST_MID[64] = {-30,-40,-40,-50,-50,-40,-40,-30,-30,-40,-40,-50,-50,-40,-40,-30,-30,-40,-40,-50,-50,-40,-40,-30,-30,-40,-40,-50,-50,-40,-40,-30,-20,-30,-30,-40,-40,-30,-30,-20,-10,-20,-20,-20,-20,-20,-20,-10,20,20,0,0,0,0,20,20,20,30,10,0,0,10,30,20};
constexpr int KING_PST_END[64] = {-50,-40,-30,-20,-20,-30,-40,-50,-30,-20,-10,0,0,-10,-20,-30,-30,-10,20,30,30,20,-10,-30,-30,-10,30,40,40,30,-10,-30,-30,-10,30,40,40,30,-10,-30,-30,-10,20,30,30,20,-10,-30,-30,-30,0,0,0,0,-30,-30,-50,-30,-30,-30,-30,-30,-30,-50};


inline int piece_eval(bitboard_t pieces, const Color &turn, const int base_value, const int pst[]) {
    int score = 0;
    while (pieces != 0) {
        const bitboard_t piece = pieces & ~pieces + 1;
        int index = std::bit_width(piece) - 1;
        if (turn == Color::Black) index ^= 56;
        score += base_value + pst[index];
        pieces ^= piece;
    }
    return score;
}


inline int state_eval(const Bitboard &state, const Color &turn) {
    const Color own_color = turn;
    const Color other_color = (turn == Color::White ? Color::Black : Color::White);
    const bitboard_t own_pieces = (turn == Color::White ? state.white : state.black);
    const bitboard_t other_pieces = (turn == Color::White ? state.black : state.white);
    int score = 0;

    score += piece_eval(own_pieces & state.pieces[0], own_color, PAWN_VALUE, PAWN_PST)
        - piece_eval(other_pieces & state.pieces[0], other_color, PAWN_VALUE, PAWN_PST);
    score += piece_eval(own_pieces & state.pieces[1], own_color, KNIGHT_VALUE, KNIGHT_PST)
        - piece_eval(other_pieces & state.pieces[1], other_color, KNIGHT_VALUE, KNIGHT_PST);
    score += piece_eval(own_pieces & state.pieces[2], own_color, BISHOP_VALUE, BISHOP_PST)
        - piece_eval(other_pieces & state.pieces[2], other_color, BISHOP_VALUE, BISHOP_PST);
    score += piece_eval(own_pieces & state.pieces[3], own_color, ROOK_VALUE, ROOK_PST)
        - piece_eval(other_pieces & state.pieces[3], other_color, ROOK_VALUE, ROOK_PST);
    score += piece_eval(own_pieces & state.pieces[4], own_color, QUEEN_VALUE, QUEEN_PST)
        - piece_eval(other_pieces & state.pieces[4], other_color, QUEEN_VALUE, QUEEN_PST);

    if ((own_pieces | other_pieces) & state.pieces[3]) {
        score += piece_eval(own_pieces & state.pieces[5], own_color, 0, KING_PST_MID)
            - piece_eval(other_pieces & state.pieces[5], other_color, 0, KING_PST_MID);
    } else {
        score += piece_eval(own_pieces & state.pieces[5], own_color, 0, KING_PST_END)
            - piece_eval(other_pieces & state.pieces[5], other_color, 0, KING_PST_END);
    }

    return score;
}


// 0 = ongoing, 1 = lost, 2 = statemate
inline int game_end(ChessEngine &engine) {
    if (!engine.get_legal_moves().empty()) return 0;
    if (engine.has_check(engine.get_turn() == Color::White ? Color::Black : Color::White)) return 1;
    return 2;
}


#endif //UTILS_H
