#ifndef CHESS_ENGINE_H
#define CHESS_ENGINE_H

#include <array>
#include <bitset>
#include <vector>
#include <cstdint>

typedef unsigned long long bitboard_t;
enum class Color { White, Black };
constexpr bitboard_t ROOK_MAGIC[64] = {0x60FFF28003BA18F9,0x7257F73004D23B03,0xBD6EAFE0C94FC51E,0xDBACF8F2DE1EA232,0x95773C45C9CC490B,0xBC3FFB80C3B6AE96,0xDEFFF1AC55AC25A1,0xAE5053EFD18AA1F1,0x76D2FD867D563ACE,0xB09191CF90BC9F05,0xF49A094B15044D1F,0x5CDC6B9FEB6B54AE,0x61FD19B2C1FE7D7D,0x88B832437765579D,0xF61FE7670F0BD567,0x66EC1575120FFC27,0x87D0E29DD49C3B62,0xC3712A69C43F11BF,0xABE5FAE380E0003C,0xE66682A30328C14F,0xDEB30900F933C956,0xFB350A9803805BFD,0x5ECA14BB87855176,0x40650B63C6A823B6,0x3FE5DE359DF91D0E,0x5842BFC1ACE47EED,0x963EBF6806B24EB8,0x14F30A700287BD42,0xFE0362C2314A60CD,0xBD7E13C5CF972C41,0xBDA0C6627EFE3AE2,0xFCBFE7983467A068,0xE8AF273A3BA3FA3D,0xE64978EE5558979,0xA76F6DC7028B4C6E,0xCA2C04ECA04FD14C,0xBD4861FCED2CC2FB,0x432AD78197492479,0x1D4F0ACA83D711B0,0xDCEB79EF3D80FA3A,0x3A9AFE663FAECA40,0x21E15B693448C838,0x91F45666998CD860,0x62739E322F503AFB,0x647612123DCE9E15,0x41034B5F5C720A36,0xB3BED73784253A56,0xA5A0A54AB6A1124B,0xDC2F59B92239F712,0x68CD9B5AC9E0C60B,0x670779D77DDCC2FA,0x679F04FA090E7567,0x406B42F4A199A338,0x7E36DD955F6A2D4E,0x22BB1040D6033B80,0x483A476ED90A9E74,0x9A93898FA951D392,0x9C3E0536EE40B6A2,0x62C9ED7188906152,0x12537C1861003ED1,0xA162B6E041CC2F8A,0x1FAB455D785158A6,0x29C057FA03F02671,0x1C4C6BB37C9C010E};
constexpr bitboard_t BISHOP_MAGIC[64] = {0x1B893094930C8CD3,0x34290BC08499D461,0x5059D30C72EC9D04,0x293584ACB08C7900,0x7406BF9FE8EE15C2,0xCAF7A6248BAFC434,0x54EE1E597405957E,0xA1C086D77F4545F8,0x7FB9F64505AD3FB0,0xE14FD300B5E58878,0x3449CE3964FD270A,0xFDE6839B7A5230B2,0xE3CD66BBF1286F38,0xB2705C232E2B18C5,0xA8CCB2774B96B2,0xF33928D3CF4AFCEC,0x1425A56EA12D9282,0x72228ED4CCFC51,0xF71CB661C3CE0D8,0xBF9C78B58D96766C,0xCE72CDC69021D7AE,0xAADAF5B8C21F6AEB,0xE8638EB3B12FEBD5,0x111D2414A855F05A,0x74630236C7299F45,0x165004D026A0321A,0x4F407EFF49D49F8F,0x9A7F119985A81040,0x938C8AB210EF7BF,0x56D6A0B8CD43AFA8,0xA082E2AF12FCF3D3,0x2B1F28AB067E2183,0xABF7B3A3EE1ACD21,0x86593032CBB6D444,0xAB4776F34F5C18C0,0x2D3F869F867C751A,0xC3B3FB13A8800BAF,0x1E5225C28AB3D704,0x6FEE1DD27C92FC72,0x479AD1E4CAAAD435,0x3C3D31E6CDC5BBC0,0x791F865A828C6F78,0xE095292F31EFBC81,0x87629D485FE6AABC,0xA8E2B4FFC609E68,0xFDCF7B254BC95AAF,0x7475BF21B71F815D,0x5D58DC9A8F3A4440,0x6FF168A4C2816C1D,0x6623B4E3F2F7923F,0xAEE2D1DC6FBE054A,0xFD9822E16FBA84B3,0x7AF568C01EEDBAAC,0xED96445ACE7FCE27,0x2F671550FAD26524,0x6F395CFFDDD5E98E,0xE969F330EE8A988,0x770A8A31CF694621,0xA6DAB4E28E9EBACE,0xA93E0DC8F1287C8F,0x4644195FA63B0008,0xA1FD385DC288331B,0x6A94EF9CECD4F953,0xF8777F51B93FDFCF};
constexpr int MAGIC_MASK = 8192;
constexpr int MAGIC_SHIFT = 51;


struct Bitboard {
    bitboard_t black, white;
    bitboard_t pieces[6]{};  // 0: pawn, 1: knight, 2: bishop, 3: rook, 4: queen, 5: king;

    static const std::array<std::array<bitboard_t, 8193>, 64> rook_moves;
    static const std::array<std::array<bitboard_t, 8193>, 64> bishop_moves;
    static const std::array<bitboard_t, 64> knight_moves;
    static const std::array<bitboard_t, 64> king_moves;

    Bitboard();
};


class ChessEngine {
// ====== Helper Structs ======

    struct MoveInfo {
        int from, to, cap, cap_piece, en_passant;
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
    int pieces[64] = {
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
    int en_passant = -1;
    int half_move = 0;  // Draw when half-move clock hits 100
    CircularStack<20> move_history;

// ====== Methods ======

    [[nodiscard]] bitboard_t attacked_cells(Color attacker) const;
    [[nodiscard]] bitboard_t get_piece_moves(int index, Color turn) const;
    void move(int from, int to, bool undoing);
public:
    [[nodiscard]] bitboard_t get_piece_moves(int index) const;  // Pseudo-legal, use has_check and unmove to filter
    [[nodiscard]] std::vector<std::pair<int, int>> get_moves() const;  // Pseudo-legal, use has_check and unmove to filter
    [[nodiscard]] bool has_check(Color attacker) const;
    [[nodiscard]] Color get_turn() const;
    void move(int from, int to);
    void unmove();
};

#endif //CHESS_ENGINE_H
