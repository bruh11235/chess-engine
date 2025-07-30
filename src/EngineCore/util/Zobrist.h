#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "Bitboard.h"
#include <array>
#include <random>


class Zobrist {
    long long val = 0;
    Color current_turn = Color::White;

    static std::mt19937 rng;
    static std::array<std::array<long long, 12>, 64> table;
    static long long toggle_turn;

    static std::array<std::array<long long, 12>, 64> init_table();
    static long long get_random_long();
public:
    Zobrist() = default;
    explicit Zobrist(const Bitboard &state);
    [[nodiscard]] long long get_value() const { return val; }
    void toggle(int index, int piece);
    void set_turn(Color turn);
};

#endif //ZOBRIST_H
