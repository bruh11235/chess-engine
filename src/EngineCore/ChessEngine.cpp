#include "ChessEngine.h"
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
                        masks[r * 8 + c] |= 1ull << r2 * 8 + c2;
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
                        full_mask |= 1ull << r2 * 8 + c2;
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
                            mask |= 1ull << r2 * 8 + c2;
                            if (subset & 1ull << r2 * 8 + c2) break;
                            r2 += dr, c2 += dc;
                        }
                    }
                    const int index = static_cast<int>(subset * magic[r * 8 + c] >> MAGIC_SHIFT);
                    assert(0 <= index < (1 << (64 - MAGIC_SHIFT)));
                    assert(!masks[r * 8 + c][index] || masks[r * 8 + c][index] == mask);
                    masks[r * 8 + c][index] = mask;
                    subset = subset - full_mask & full_mask;
                } while (subset != 0);
            }
        }
        return masks;
    }
};


const array<array<bitboard_t, 8193>, 64> Bitboard::rook_moves = generate_sweeping_moves({{-1, 0}, {1, 0}, {0, -1}, {0, 1}}, ROOK_MAGIC);
const array<array<bitboard_t, 8193>, 64> Bitboard::bishop_moves = generate_sweeping_moves({{1, 1}, {-1, 1}, {1, -1}, {-1, -1}}, BISHOP_MAGIC);
const array<bitboard_t, 64> Bitboard::knight_moves = generate_fixed_moves({{1, 2}, {-1, 2}, {1, -2}, {-1, -2}, {2, 1}, {-2, 1}, {2, -1}, {-2, -1}});
const array<bitboard_t, 64> Bitboard::king_moves = generate_fixed_moves({{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {1, 1}, {-1, 1}, {1, -1}, {-1, -1}});


ChessEngine::ChessEngine(const string& fen) {
    istringstream iss(fen);
    string pos, turn, castling, en_passant, half, full;
    iss >> pos >> turn >> castling >> en_passant >> half >> full;

    // Position
    for (int &piece : pieces) piece = 6;
    int index = 0;
    for (char c : pos) {
        if (c == '/') continue;
        if ('0' <= c && c <= '9') {
            index += (c - '0');
        } else {
            if ('a' <= c && c <= 'z') state.black |= (1ull << index);
            if ('A' <= c && c <= 'Z') state.white |= (1ull << index);
            if (tolower(c) == 'p') {
                state.pieces[0] |= (1ull << index);
                pieces[index] = 0;
            }
            if (tolower(c) == 'n') {
                state.pieces[1] |= (1ull << index);
                pieces[index] = 1;
            }
            if (tolower(c) == 'b') {
                state.pieces[2] |= (1ull << index);
                pieces[index] = 2;
            }
            if (tolower(c) == 'r') {
                state.pieces[3] |= (1ull << index);
                pieces[index] = 3;
            }
            if (tolower(c) == 'q') {
                state.pieces[4] |= (1ull << index);
                pieces[index] = 4;
            }
            if (tolower(c) == 'k') {
                state.pieces[5] |= (1ull << index);
                pieces[index] = 5;
            }
            index++;
        }
    }

    // Turn
    this->turn = (turn == "w" ? Color::White : Color::Black);

    // Castling
    for (int i = 0; i < 4; ++i) {
        if (castling.find("KQkq"[i]) != std::string::npos) {
            this->castling[i] = true;
        }
    }

    // En passant
    if (en_passant == "-") {
        this->en_passant = -1;
    } else {
        this->en_passant = (8 - en_passant[1] + '0') * 8 + (en_passant[0] - 'a');
    }
}


bitboard_t ChessEngine::get_piece_moves(const int index, const Color turn) const {
    bitboard_t moves = 0;

    const bitboard_t piece = 1ull << index;
    const bitboard_t all_pieces = state.black | state.white;
    const bitboard_t own_pieces = (turn == Color::Black ? state.black : state.white);
    if (!(own_pieces & piece)) return 0;

    // Pawns
    if (state.pieces[0] & own_pieces & piece) {
        if (turn == Color::Black) {
            if ((index & 7) != 0 && ((all_pieces & piece << 7) || (index + 7 == en_passant))) moves |= piece << 7;
            if ((index & 7) != 7 && ((all_pieces & piece << 9) || (index + 9 == en_passant))) moves |= piece << 9;
            if (!(all_pieces & piece << 8)) {
                moves |= piece << 8;
                if ((index >> 3) == 1 && !(all_pieces & piece << 16)) moves |= piece << 16;
            }
        } else {
            if ((index & 7) != 7 && ((all_pieces & piece >> 7) || (index - 7 == en_passant))) moves |= piece >> 7;
            if ((index & 7) != 0 && ((all_pieces & piece >> 9) || (index - 9 == en_passant))) moves |= piece >> 9;
            if (!(all_pieces & piece >> 8)) {
                moves |= piece >> 8;
                if ((index >> 3) == 6 && !(all_pieces & piece >> 16)) moves |= piece >> 16;
            }
        }
    }

    // Rook & Queen1
    if ((state.pieces[4] | state.pieces[3]) & own_pieces & piece) {
        const bitboard_t occupancy = all_pieces & Bitboard::rook_moves[index][MAGIC_MASK];
        moves |= Bitboard::rook_moves[index][occupancy * ROOK_MAGIC[index] >> MAGIC_SHIFT];
    }

    // Bishop & Queen2
    if ((state.pieces[4] | state.pieces[2]) & own_pieces & piece) {
        const bitboard_t occupancy = all_pieces & Bitboard::bishop_moves[index][MAGIC_MASK];
        moves |= Bitboard::bishop_moves[index][occupancy * BISHOP_MAGIC[index] >> MAGIC_SHIFT];
    }

    // Knight
    if (state.pieces[1] & own_pieces & piece) {
        moves |= Bitboard::knight_moves[index];
    }

    // King
    if (state.pieces[5] & own_pieces & piece) {
        moves |= Bitboard::king_moves[index];
        if (turn == Color::White && !has_check(Color::Black)) {
            if ((state.white & (state.pieces[3] | state.pieces[5]) & 0b1111ull << 60) == (1ull << 60 | 1ull << 63)
                && (all_pieces & ((1ull << 61) | (1ull << 62))) == 0 && castling[0] && !is_attacked(Color::Black, piece << 1))
                moves |= piece << 2;
            if ((state.white & (state.pieces[3] | state.pieces[5]) & 0b11111ull << 56) == (1ull << 56 | 1ull << 60)
            && (all_pieces & ((1ull << 57) | (1ull << 58) | (1ull << 59))) == 0 && castling[1] && !is_attacked(Color::Black, piece >> 1))
                moves |= piece >> 2;
        } else if (turn == Color::Black && !has_check(Color::White)) {
            if ((state.black & (state.pieces[3] | state.pieces[5]) & 0b1111 << 4) == (1 << 4 | 1 << 7)
                && (all_pieces & ((1 << 5) | (1 << 6))) == 0 && castling[2] && !is_attacked(Color::White, piece << 1))
                moves |= piece << 2;
            if ((state.black & (state.pieces[3] | state.pieces[5]) & 31) == 17 &&
                (all_pieces & 14) == 0 && castling[3] && !is_attacked(Color::White, piece >> 1))
                moves |= piece >> 2;
        }
    }

    return moves ^ moves & own_pieces;
}


bitboard_t ChessEngine::get_piece_moves(const int index) const {
    return get_piece_moves(index, turn);
}


// Return: [from, to, promotion_choices]
vector<tuple<int, int, int>> ChessEngine::get_moves() const {
    vector<tuple<int, int, int>> moves;
    bitboard_t own_pieces = (turn == Color::Black ? state.black : state.white);
    while (own_pieces != 0) {
        const bitboard_t from = own_pieces & ~own_pieces + 1;

        bitboard_t possible_moves = get_piece_moves(bit_width(from) - 1);
        while (possible_moves != 0) {
            const bitboard_t to = possible_moves & ~possible_moves + 1;
            const int from_id = bit_width(from) - 1, to_id = bit_width(to) - 1;
            if (turn == Color::Black && (to_id >> 3) == 7 && (state.pieces[0] & from) ||
                turn == Color::White && (to_id >> 3) == 0 && (state.pieces[0] & from)) {
                moves.emplace_back(from_id, to_id, 1);
                moves.emplace_back(from_id, to_id, 2);
                moves.emplace_back(from_id, to_id, 3);
                moves.emplace_back(from_id, to_id, 4);
            } else {
                moves.emplace_back(from_id, to_id, -1);
            }

            possible_moves ^= to;
        }

        own_pieces ^= from;
    }
    return moves;
}


vector<tuple<int, int, int>> ChessEngine::get_legal_moves() {
    vector<tuple<int, int, int>> moves = get_moves();
    vector<tuple<int, int, int>> legal_moves;

    for (auto [from, to, promote] : moves) {
        move(from, to, promote, false);
        if (!has_check(turn)) {
            legal_moves.emplace_back(from, to, promote);
        }
        unmove();
    }

    return legal_moves;
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


bool ChessEngine::is_attacked(const Color attacker, const bitboard_t attacked_cell) const {
    const bitboard_t all_pieces = state.black | state.white;
    const bitboard_t attacker_pieces = (attacker == Color::Black ? state.black : state.white);
    const int index = bit_width(attacked_cell) - 1;

    // Attacked by rook / queen
    const bitboard_t rook_occupancy = (all_pieces & Bitboard::rook_moves[index][MAGIC_MASK]);
    const bitboard_t rook_attacks = Bitboard::rook_moves[index][rook_occupancy * ROOK_MAGIC[index] >> MAGIC_SHIFT];
    if (rook_attacks & attacker_pieces & (state.pieces[3] | state.pieces[4])) return true;

    // Attacked by bishop / queen
    const bitboard_t bishop_occupancy = (all_pieces & Bitboard::bishop_moves[index][MAGIC_MASK]);
    const bitboard_t bishop_attacks = Bitboard::bishop_moves[index][bishop_occupancy * BISHOP_MAGIC[index] >> MAGIC_SHIFT];
    if (bishop_attacks & attacker_pieces & (state.pieces[2] | state.pieces[4])) return true;

    // Attacked by king / knight
    if (Bitboard::king_moves[index] & attacker_pieces & state.pieces[5]) return true;
    if (Bitboard::knight_moves[index] & attacker_pieces & state.pieces[1]) return true;

    // Attacked by pawn
    if (attacker == Color::White) {
        if ((index & 7) != 0 && (attacker_pieces & state.pieces[0] & (attacked_cell << 7))) return true;
        if ((index & 7) != 7 && (attacker_pieces & state.pieces[0] & (attacked_cell << 9))) return true;
    } else {
        if ((index & 7) != 0 && (attacker_pieces & state.pieces[0] & (attacked_cell >> 9))) return true;
        if ((index & 7) != 7 && (attacker_pieces & state.pieces[0] & (attacked_cell >> 7))) return true;
    }
    return false;
}


bool ChessEngine::has_check(const Color attacker) const {
    const bitboard_t target_king = (attacker == Color::Black ? state.white : state.black) & state.pieces[5];
    return is_attacked(attacker, target_king);
}


Color ChessEngine::get_turn() const { return turn; }


Bitboard ChessEngine::get_state() const { return state; }


void ChessEngine::move(const int from, const int to, const int promote_to, const bool is_undoing) {
    if (pieces[from] == 6) return;
    int cap = to, cap_piece = pieces[to];
    bool castled = false, promoted = false;
    const int old_en_passant = en_passant;
    const bitset<4> old_castling = castling;
    const bool is_black_turn = state.black & 1ull << from;

    (is_black_turn ? state.white : state.black) &= ~(1ull << to);
    (is_black_turn ? state.black : state.white) |= 1ull << to;
    if (pieces[to] < 6) state.pieces[pieces[to]] ^= 1ull << to;
    if (pieces[from] < 6) state.pieces[pieces[from]] |= 1ull << to;
    pieces[to] = pieces[from];

    (is_black_turn ? state.black : state.white) ^= 1ull << from;
    if (pieces[from] < 6) state.pieces[pieces[from]] ^= 1ull << from;
    pieces[from] = 6;

    // En Passant
    if (!is_undoing) {
        if (pieces[to] == 0 && to == en_passant) {
            cap = (to >> 3) == 2 ? to + 8 : to - 8;
            cap_piece = pieces[cap];
            (is_black_turn ? state.white : state.black) ^= 1ull << cap;
            state.pieces[0] ^= 1ull << cap;
            pieces[cap] = 6;
        }
        if (pieces[to] == 0 && abs(to - from) == 16) en_passant = (to + from) >> 1;
        else en_passant = -1;
    }

    // Castling
    if (!is_undoing) {
        if (pieces[to] == 5 && abs(from - to) == 2) {
            if ((to & 7) == 2) move((from & ~7), (from + to) >> 1, -1, false);
            else move((from & ~7) + 7, (from + to) >> 1, -1, false);
            castled = true;
            turn = (turn == Color::White ? Color::Black : Color::White);
        }
        if (!(state.white & state.pieces[3] & (1ull << 63)) || !(state.white & state.pieces[5] & (1ull << 60))) castling[0] = false;
        if (!(state.white & state.pieces[3] & (1ull << 56)) || !(state.white & state.pieces[5] & (1ull << 60))) castling[1] = false;
        if (!(state.black & state.pieces[3] & (1ull << 7)) || !(state.black & state.pieces[5] & (1ull << 4))) castling[2] = false;
        if (!(state.black & state.pieces[3] & (1ull << 0)) || !(state.black & state.pieces[5] & (1ull << 4))) castling[3] = false;
    }

    // Pawn promotion
    if (promote_to != -1 && !is_undoing) {
        promoted = true;
        state.pieces[pieces[to]] ^= (1ull << to);
        state.pieces[promote_to] ^= (1ull << to);
        pieces[to] = promote_to;
    }

    if (!is_undoing) {
        move_history.push({from, to, cap, cap_piece, old_en_passant, castled, promoted, old_castling});
        turn = (turn == Color::White ? Color::Black : Color::White);
    }
}


void ChessEngine::move(const int from, const int to, const int promote_to) { move(from, to, promote_to, false); }


void ChessEngine::unmove() {
    auto [from, to, cap, cap_piece, old_en_passant, castled, promoted, old_castling] = move_history.pop();
    move(to, from, -1, true);

    // Uncapture
    const bool black_moved = state.black & 1ull << from;
    if (cap_piece < 6) {
        (black_moved ? state.white : state.black) |= 1ull << cap;
        state.pieces[cap_piece] |= 1ull << cap;
    }
    pieces[cap] = cap_piece;

    // Unpromote
    if (promoted) {
        state.pieces[pieces[from]] ^= (1ull << from);
        state.pieces[0] |= (1ull << from);
        pieces[from] = 0;
    }

    if (castled) {
        unmove();
    } else {
        turn = (turn == Color::White ? Color::Black : Color::White);
    }

    en_passant = old_en_passant;
    castling = old_castling;
}
