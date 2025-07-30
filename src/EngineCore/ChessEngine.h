#ifndef CHESS_ENGINE_H
#define CHESS_ENGINE_H

#include "util/Zobrist.h"
#include "util/Bitboard.h"
#include <stack>
#include <bitset>
#include <vector>


class ChessEngine {
// ====== Helper Structs ======

    struct MoveInfo {
        int from{}, to{}, cap{}, cap_piece{}, en_passant{};
        bool castled{}, promoted{};
        std::bitset<4> castling;
    };

// ====== Attributes ======

    Bitboard state;
    int pieces[64]{};  // 0: pawn, 1: knight, 2: bishop, 3: rook, 4: queen, 5: king, 6: empty;
    Zobrist hash;
    Color turn;
    int en_passant;
    std::bitset<4> castling;  // (white ->) KQkq (<- black)
    std::stack<MoveInfo> move_history;

// ====== Methods ======

    [[nodiscard]] bitboard_t attacked_cells(Color attacker) const;
    [[nodiscard]] bitboard_t get_piece_moves(int index, Color turn) const;
    [[nodiscard]] bool is_attacked(Color attacker, bitboard_t attacked_cell) const;
    [[nodiscard]] bitboard_t get_piece_moves(int index) const;  // Pseudo-legal
    [[nodiscard]] std::vector<std::tuple<int, int, int>> get_moves() const;  // Pseudo-legal
    void move(int from, int to, int promote_to, bool is_undoing);
public:
    explicit ChessEngine(const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    [[nodiscard]] std::vector<std::tuple<int, int, int>> get_legal_moves();
    [[nodiscard]] bool has_check(Color attacker) const;
    [[nodiscard]] Color get_turn() const;
    [[nodiscard]] Bitboard get_state() const;
    [[nodiscard]] long long get_hash() const { return hash.get_value(); }
    void move(int from, int to, int promote_to = -1);  // promote_to: [1: knight, 2: bishop, 3: rook, 4: queen]
    void unmove();
};

#endif //CHESS_ENGINE_H
