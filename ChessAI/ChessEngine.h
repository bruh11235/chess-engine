#ifndef CHESS_ENGINE_H
#define CHESS_ENGINE_H

#include <array>
#include <bitset>
#include <vector>
#include <cstdint>
#include <unordered_map>

typedef unsigned long long bitboard_t;
enum class Color { White, Black };


struct Bitboard {
    bitboard_t black, white;
    bitboard_t pieces[6]{};  // 0: pawn, 1: knight, 2: bishop, 3: rook, 4: queen, 5: king;

    static const std::array<std::unordered_map<bitboard_t, bitboard_t>, 64> rook_moves;
    static const std::array<std::unordered_map<bitboard_t, bitboard_t>, 64> bishop_moves;
    static const std::array<bitboard_t, 64> knight_moves;
    static const std::array<bitboard_t, 64> king_moves;

    Bitboard();
};


class ChessEngine {
// ====== Helper Structs ======

    struct MoveInfo {
        int8_t from, to, cap, cap_piece;
    };
    template <std::size_t N>
    struct CircularStack {
        MoveInfo arr[N]{};
        int count = 0, bottom = 0;
        void push(const MoveInfo& value) {
            if (count < N) {
                arr[(bottom + count++) % N] = value;
            } else {
                arr[bottom] = value;
                bottom = (bottom + 1) % N;
            }
        }
        MoveInfo pop() { return arr[(bottom + --count) % N]; }
    };

// ====== Attributes ======

    Bitboard state;
    int8_t pieces[64] = {
        3,1,2,4,5,2,1,3,
        0,0,0,0,0,0,0,0,
        6,6,6,6,6,6,6,6,
        6,6,6,6,6,6,6,6,
        6,6,6,6,6,6,6,6,
        6,6,6,6,6,6,6,6,
        0,0,0,0,0,0,0,0,
        3,1,2,4,5,2,1,3
    };  // 0: pawn, 1: knight, 2: bishop, 3: rook, 4: queen, 5: king, 6: empty;

    Color turn = Color::White;
    std::bitset<4> castling{"1111"};  // (white ->) KQkq (<- black)
    short en_passant = -1;
    short half_move = 0;  // Draw when half-move clock hits 100
    CircularStack<20> move_history;

// ====== Methods ======

    [[nodiscard]] bitboard_t attacked_cells(Color attacker) const;
    [[nodiscard]] bitboard_t get_piece_moves(int index, Color turn) const;
public:
    [[nodiscard]] bitboard_t get_piece_moves(int index) const;  // Pseudo-legal, use has_check and unmove to filter
    [[nodiscard]] std::vector<std::pair<int, int>> get_moves() const;  // Pseudo-legal, use has_check and unmove to filter
    [[nodiscard]] bool has_check(Color attacker) const;
    [[nodiscard]] Color get_turn() const;
    void move(int from, int to, bool save_to_history = true);
    void unmove();
};

#endif //CHESS_ENGINE_H
