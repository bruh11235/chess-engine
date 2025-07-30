#include "Zobrist.h"
#include <bits/stdc++.h>
using namespace std;

namespace {
    constexpr int RNG_SEED = 0;
}


long long Zobrist::get_random_long() {
    uniform_int_distribution<long long> dist(LLONG_MIN, LLONG_MAX);
    return dist(rng);
}


array<array<long long, 12>, 64> Zobrist::init_table() {
    array<array<long long, 12>, 64> table{};
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 12; j++) {
            table[i][j] = get_random_long();
        }
    }
    return table;
}


mt19937 Zobrist::rng = mt19937(RNG_SEED);
array<array<long long, 12>, 64> Zobrist::table = init_table();
long long Zobrist::toggle_turn = get_random_long();


Zobrist::Zobrist(const Bitboard &state) {
    for (int i = 0; i < 64; i++) {
        int piece = -1;
        for (int j = 0; j < 6; j++) {
            if (state.pieces[j] & (1ull << i)) piece = j;
        }
        if (state.black & (1ull << i)) piece += 6;
        if (piece != -1) {
            val ^= table[i][piece];
        }
    }
}


void Zobrist::toggle(const int index, const int piece) {
    val ^= table[index][piece];
}


void Zobrist::set_turn(const Color turn) {
    if (turn != current_turn) val ^= toggle_turn;
    current_turn = turn;
}
