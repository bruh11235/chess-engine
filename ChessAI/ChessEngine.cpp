#include "ChessEngine.h"
#include <bits/stdc++.h>
using namespace std;

constexpr bitboard_t ALL_ONE = ~0ull;

// Internal helpers
namespace {
    array<bitboard_t, 64> generate_fixed_moves(const vector<pair<int, int>> &moves) {
        array<bitboard_t, 64> masks{};
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                for (auto [dr, dc] : moves) {
                    if (const int r2 = r + dr, c2 = c + dc; 0 <= r2 && r2 < 8 && 0 <= c2 && c2 < 8)
                        masks[r * 8 + c] |= 1ull << r2 * 8 + c2;
                }
            }
        }
        return masks;
    }


    /*
        array[i][ALL_ONE] = All relevant occupancy positions
    */
    array<unordered_map<bitboard_t, bitboard_t>, 64> generate_sweeping_moves(const vector<pair<int, int>> &moves) {
        array<unordered_map<bitboard_t, bitboard_t>, 64>masks{};
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                bitboard_t full_mask = 0;
                for (auto [dr, dc] : moves) {
                    int r2 = r + dr, c2 = c + dc;
                    while (0 <= r2 + dr && r2 + dr < 8 && 0 <= c2 + dc && c2 + dc < 8) {
                        full_mask |= 1ull << r2 * 8 + c2;
                        r2 += dr, c2 += dc;
                    }
                }
                masks[r * 8 + c].insert_or_assign(ALL_ONE, full_mask);

                bitboard_t subset = 0;
                do {
                    bitboard_t mask = 0;
                    for (auto [dr, dc] : moves) {
                        int r2 = r + dr, c2 = c + dc;
                        while (0 <= r2 && r2 < 8 && 0 <= c2 && c2 < 8) {
                            mask |= 1ull << r2 * 8 + c2;
                            if (subset & 1ull << r2 * 8 + c2) break;
                            r2 += dr, c2 += dc;
                        }
                    }
                    masks[r * 8 + c].insert_or_assign(subset, mask);
                    subset = subset - full_mask & full_mask;
                } while (subset != 0);
            }
        }
        return masks;
    }
};


const array<unordered_map<bitboard_t, bitboard_t>, 64> Bitboard::rook_moves = generate_sweeping_moves({{-1, 0}, {1, 0}, {0, -1}, {0, 1}});
const array<unordered_map<bitboard_t, bitboard_t>, 64> Bitboard::bishop_moves = generate_sweeping_moves({{1, 1}, {-1, 1}, {1, -1}, {-1, -1}});
const array<bitboard_t, 64> Bitboard::knight_moves = generate_fixed_moves({{1, 2}, {-1, 2}, {1, -2}, {-1, -2}, {2, 1}, {-2, 1}, {2, -1}, {-2, -1}});
const array<bitboard_t, 64> Bitboard::king_moves = generate_fixed_moves({{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {1, 1}, {-1, 1}, {1, -1}, {-1, -1}});


Bitboard::Bitboard() {
    black=      0x000000000000FFFF;
    white=      0xFFFF000000000000;
    pieces[0]=  0x00FF00000000FF00;
    pieces[1]=  0x4200000000000042;
    pieces[2]=  0x2400000000000024;
    pieces[3]=  0x8100000000000081;
    pieces[4]=  0x0800000000000008;
    pieces[5]=  0x1000000000000010;
}


// TODO: En passant, Pawn Promotion, Castling
bitboard_t ChessEngine::get_piece_moves(const int index, const Color turn) const {
    bitboard_t moves = 0;

    const bitboard_t piece = 1ull << index;
    const bitboard_t all_pieces = state.black | state.white;
    const bitboard_t own_pieces = (turn == Color::Black ? state.black : state.white);
    if (!(own_pieces & piece)) return 0;

    // Pawns
    if (state.pieces[0] & own_pieces & piece) {
        if (turn == Color::Black) {
            if ((index % 8 != 0) && (all_pieces & piece << 7)) moves |= piece << 7;
            if ((index % 8 != 7) && (all_pieces & piece << 9)) moves |= piece << 9;
            if (!(all_pieces & piece << 8)) {
                moves |= piece << 8;
                if ((index / 8 == 1) && !(all_pieces & piece << 16)) moves |= piece << 16;
            }
        } else {
            if ((index % 8 != 7) && (all_pieces & piece >> 7)) moves |= piece >> 7;
            if ((index % 8 != 0) && (all_pieces & piece >> 9)) moves |= piece >> 9;
            if (!(all_pieces & piece >> 8)) {
                moves |= piece >> 8;
                if ((index / 8 == 6) && !(all_pieces & piece >> 16)) moves |= piece >> 16;
            }
        }
    }

    // Rook & Queen1
    if ((state.pieces[4] | state.pieces[3]) & own_pieces & piece) {
        const bitboard_t occupancy = all_pieces & Bitboard::rook_moves[index].at(ALL_ONE);
        moves |= Bitboard::rook_moves[index].at(occupancy);
    }

    // Bishop & Queen2
    if ((state.pieces[4] | state.pieces[2]) & own_pieces & piece) {
        const bitboard_t occupancy = all_pieces & Bitboard::bishop_moves[index].at(ALL_ONE);
        moves |= Bitboard::bishop_moves[index].at(occupancy);
    }

    // Knight
    if (state.pieces[1] & own_pieces & piece) {
        moves |= Bitboard::knight_moves[index];
    }

    // King
    if (state.pieces[5] & own_pieces & piece) {
        moves |= Bitboard::king_moves[index];
    }

    return moves ^ moves & own_pieces;
}


bitboard_t ChessEngine::get_piece_moves(const int index) const {
    return get_piece_moves(index, turn);
}


vector<pair<int, int>> ChessEngine::get_moves() const {
    vector<pair<int, int>> moves;
    bitboard_t own_pieces = (turn == Color::Black ? state.black : state.white);
    while (own_pieces != 0) {
        const bitboard_t from = own_pieces & ~own_pieces + 1;

        bitboard_t possible_moves = get_piece_moves(bit_width(from) - 1);
        while (possible_moves != 0) {
            const bitboard_t to = possible_moves & ~possible_moves + 1;
            moves.emplace_back(bit_width(from) - 1, bit_width(to) - 1);
            possible_moves ^= to;
        }

        own_pieces ^= from;
    }
    return moves;
}


bitboard_t ChessEngine::attacked_cells(const Color attacker) const {
    bitboard_t moves = 0;
    const bitboard_t own_pieces = (attacker == Color::Black ? state.black : state.white);
    bitboard_t current_pieces = own_pieces;

    while (current_pieces != 0) {
        const bitboard_t piece = current_pieces & ~current_pieces + 1;
        moves |= get_piece_moves(bit_width(piece) - 1);
        current_pieces ^= piece;
    }
    moves ^= moves & own_pieces;

    return moves;
}


bool ChessEngine::has_check(const Color attacker) const {
    const bitboard_t attacked = attacked_cells(attacker);
    const bitboard_t other_king = (attacker == Color::Black ? state.white : state.black) & state.pieces[5];
    return attacked & other_king;
}


Color ChessEngine::get_turn() const { return turn; }


void ChessEngine::move(const int from, const int to, const bool save_to_history) {
    if (pieces[from] == 6) return;
    const int8_t captured = pieces[to];
    const bool is_black_turn = state.black & 1ull << from;

    (is_black_turn ? state.white : state.black) &= ~(1ull << to);
    (is_black_turn ? state.black : state.white) |= 1ull << to;
    if (pieces[to] < 6) state.pieces[pieces[to]] ^= 1ull << to;
    if (pieces[from] < 6) state.pieces[pieces[from]] |= 1ull << to;
    pieces[to] = pieces[from];

    (is_black_turn ? state.black : state.white) ^= 1ull << from;
    if (pieces[from] < 6) state.pieces[pieces[from]] ^= 1ull << from;
    pieces[from] = 6;

    if (save_to_history) {
        const auto from8 = static_cast<int8_t>(from), to8 = static_cast<int8_t>(to);
        move_history.push({from8, to8, to8, captured});
    }
    turn = (turn == Color::White ? Color::Black : Color::White);
}


void ChessEngine::unmove() {
    auto [from, to, cap, cap_piece] = move_history.pop();
    move(to, from, false);

    const bool black_moved = state.black & 1ull << from;
    if (cap_piece < 6) {
        (black_moved ? state.white : state.black) |= 1ull << cap;
        state.pieces[cap_piece] |= 1ull << cap;
    }
    pieces[cap] = cap_piece;
}
