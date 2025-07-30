#include "Bitboard.h"
#include <bits/stdc++.h>
using namespace std;

// Internal helpers
namespace {
    array<bitboard_t, 64> generate_fixed_moves(const vector<pair<int, int>> &moves) {
        array<bitboard_t, 64> masks{};
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                for (auto [dr, dc] : moves) {
                    if (const int r2 = r + dr, c2 = c + dc; 0 <= r2 && r2 < 8 && 0 <= c2 && c2 < 8)
                        masks[r * 8 + c] |= 1ull << (r2 * 8 + c2);
                }
            }
        }
        return masks;
    }


    /*
        array[i][MAGIC_MASK] = All relevant occupancy positions
    */
    array<array<bitboard_t, 8193>, 64> generate_sweeping_moves(const vector<pair<int, int>> &moves, const bitboard_t magic[64]) {
        array<array<bitboard_t, 8193>, 64>masks{};
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                bitboard_t full_mask = 0;
                for (auto [dr, dc] : moves) {
                    int r2 = r + dr, c2 = c + dc;
                    while (0 <= r2 + dr && r2 + dr < 8 && 0 <= c2 + dc && c2 + dc < 8) {
                        full_mask |= 1ull << (r2 * 8 + c2);
                        r2 += dr, c2 += dc;
                    }
                }
                masks[r * 8 + c][MAGIC_MASK] = full_mask;

                bitboard_t subset = 0;
                do {
                    bitboard_t mask = 0;
                    for (auto [dr, dc] : moves) {
                        int r2 = r + dr, c2 = c + dc;
                        while (0 <= r2 && r2 < 8 && 0 <= c2 && c2 < 8) {
                            mask |= 1ull << (r2 * 8 + c2);
                            if (subset & 1ull << (r2 * 8 + c2)) break;
                            r2 += dr, c2 += dc;
                        }
                    }
                    const int index = static_cast<int>(subset * magic[r * 8 + c] >> MAGIC_SHIFT);
                    assert(0 <= index && index < (1 << (64 - MAGIC_SHIFT)));
                    assert(!masks[r * 8 + c][index] || masks[r * 8 + c][index] == mask);
                    masks[r * 8 + c][index] = mask;
                    subset = (subset - full_mask) & full_mask;
                } while (subset != 0);
            }
        }
        return masks;
    }
}


const array<array<bitboard_t, 8193>, 64> Bitboard::rook_moves = generate_sweeping_moves({{-1, 0}, {1, 0}, {0, -1}, {0, 1}}, ROOK_MAGIC);
const array<array<bitboard_t, 8193>, 64> Bitboard::bishop_moves = generate_sweeping_moves({{1, 1}, {-1, 1}, {1, -1}, {-1, -1}}, BISHOP_MAGIC);
const array<bitboard_t, 64> Bitboard::knight_moves = generate_fixed_moves({{1, 2}, {-1, 2}, {1, -2}, {-1, -2}, {2, 1}, {-2, 1}, {2, -1}, {-2, -1}});
const array<bitboard_t, 64> Bitboard::king_moves = generate_fixed_moves({{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {1, 1}, {-1, 1}, {1, -1}, {-1, -1}});
