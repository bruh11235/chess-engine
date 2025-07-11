"""
Contains the ChessModel class.
"""
from __future__ import annotations

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from ChessView import ChessView

KNIGHT_MOVES = ((1, 2), (2, 1), (-1, 2), (-2, 1), (1, -2), (2, -1), (-1, -2), (-2, -1))
DIR8 = ((1, 0), (-1, 0), (0, 1), (0, -1), (1, 1), (1, -1), (-1, 1), (-1, -1))


def _deepcopy(state: list[list[str]]) -> list[list[str]]:
    new_state = []
    for r, row in enumerate(state):
        new_state.append([])
        for cell in row:
            new_state[r].append(cell)
    return new_state


class ChessModel:
    """
    Represents the data and game logic layer.
    """

    def __init__(self):
        self.view: ChessView | None = None
        self.turn: str = 'w'
        self.castling: str = 'KQkq'
        self.en_passant: tuple | None = None
        self.half_move: int = 0
        self.full_move: int = 0
        self.state = [['r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'],
                      ['p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'],  # Black
                      ['.', '.', '.', '.', '.', '.', '.', '.'],
                      ['.', '.', '.', '.', '.', '.', '.', '.'],
                      ['.', '.', '.', '.', '.', '.', '.', '.'],
                      ['.', '.', '.', '.', '.', '.', '.', '.'],
                      ['P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'],
                      ['R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R']]  # White
        self.selected: tuple | None = None
        self.promotion: tuple | None = None

    def get_piece_moves(self, row: int, col: int, lookahead: bool=True) -> list:
        """
        Returns a list of moves of the piece specified by (`row`, `col`)

        If `lookahead` is true then moves that result in immediately loss will be discarded
        """
        if not(0 <= row < 8 and 0 <= col < 8) or self.state[row][col] == '.':
            return []

        color = 'w' if self.state[row][col].isupper() else 'b'
        pseudo_moves = []

        # ===== Pawn =====
        if self.state[row][col].upper() == 'P':
            at_home = (color == 'w' and row == 6) or (color == 'b' and row == 1)
            direction = 1 if color == 'b' else -1

            if self.state[row + direction][col] == '.':  # Move forwards
                pseudo_moves.append((row + direction, col))
                if at_home and self.state[row + direction * 2][col] == '.':
                    pseudo_moves.append((row + direction * 2, col))

            for side in [-1, 1]:  # Capture
                if not(0 <= col + side < 8):
                    continue
                if self.state[row + direction][col + side].isalpha():
                    other_color = 'w' if self.state[row + direction][col + side].isupper() else 'b'
                    if color != other_color:
                        pseudo_moves.append((row + direction, col + side))
                if (row + direction, col + side) == self.en_passant:
                    pseudo_moves.append((row + direction, col + side))

        # ===== Rook =====
        elif self.state[row][col].upper() == 'R':
            for dx, dy in DIR8[:4]:
                cx, cy = row, col
                while True:
                    cx += dx
                    cy += dy
                    if not(0 <= cx < 8 and 0 <= cy < 8):
                        break
                    if self.state[cx][cy].isalpha():
                        other_color = 'w' if self.state[cx][cy].isupper() else 'b'
                        if color != other_color:
                            pseudo_moves.append((cx, cy))
                        break
                    pseudo_moves.append((cx, cy))

        # ===== Bishop =====
        elif self.state[row][col].upper() == 'B':
            for dx, dy in DIR8[4:]:
                cx, cy = row, col
                while True:
                    cx += dx
                    cy += dy
                    if not(0 <= cx < 8 and 0 <= cy < 8):
                        break
                    if self.state[cx][cy].isalpha():
                        other_color = 'w' if self.state[cx][cy].isupper() else 'b'
                        if color != other_color:
                            pseudo_moves.append((cx, cy))
                        break
                    pseudo_moves.append((cx, cy))

        # ===== Queen =====
        elif self.state[row][col].upper() == 'Q':
            for dx, dy in DIR8:
                cx, cy = row, col
                while True:
                    cx += dx
                    cy += dy
                    if not(0 <= cx < 8 and 0 <= cy < 8):
                        break
                    if self.state[cx][cy].isalpha():
                        other_color = 'w' if self.state[cx][cy].isupper() else 'b'
                        if color != other_color:
                            pseudo_moves.append((cx, cy))
                        break
                    pseudo_moves.append((cx, cy))

        # ===== Knight =====
        elif self.state[row][col].upper() == 'N':
            for dx, dy in KNIGHT_MOVES:
                if not(0 <= row + dx < 8 and 0 <= col + dy < 8):
                    continue
                other_color = 'w' if self.state[row + dx][col + dy].isupper() else 'b'
                if self.state[row + dx][col + dy] == '.' or other_color != color:
                    pseudo_moves.append((row + dx, col + dy))

        # ===== King =====
        elif self.state[row][col].upper() == 'K':
            for dx, dy in DIR8:
                if not(0 <= row + dx < 8 and 0 <= col + dy < 8):
                    continue
                other_color = 'w' if self.state[row + dx][col + dy].isupper() else 'b'
                if self.state[row + dx][col + dy] == '.' or other_color != color:
                    pseudo_moves.append((row + dx, col + dy))
            if color == 'b' and 'q' in self.castling and self.state[0][:5] == ['r','.','.','.','k']:
                pseudo_moves.append((0, 2))
            if color == 'b' and 'k' in self.castling and self.state[0][4:] == ['k','.','.','r']:
                pseudo_moves.append((0, 6))
            if color == 'w' and 'Q' in self.castling and self.state[7][:5] == ['R','.','.','.','K']:
                pseudo_moves.append((7, 2))
            if color == 'w' and 'Q' in self.castling and self.state[7][4:] == ['K','.','.','R']:
                pseudo_moves.append((7, 6))

        # Check for unblocked checks
        if lookahead:
            moves = []
            for r2, c2 in pseudo_moves:
                if self.try_move(row, col, r2, c2):
                    moves.append((r2, c2))
            return moves
        else:
            return pseudo_moves

    def get_moves(self, player: str) -> list:
        """
        Returns a list of all possible (pseudo) moves for `player`
        """
        moves = []
        for r in range(8):
            for c in range(8):
                color = 'w' if self.state[r][c].isupper() else 'b'
                if self.state[r][c].isalpha() and color == player:
                    cell_moves = self.get_piece_moves(r, c, False)
                    for move in cell_moves:
                        if move not in moves:
                            moves.append(move)
        return moves

    def get_state(self) -> list[list]:
        """
        Returns the current state of the board as a 2D str array
        """
        return self.state

    def check_check(self, player: str) -> bool:
        """
        Check if `player` can capture the opponent's king
        """
        moves = self.get_moves(player)
        for r, c in moves:
            if self.state[r][c].upper() == 'K':
                return True
        return False

    def try_move(self, r1: int, c1: int, r2: int, c2: int) -> bool:
        """
        Try to move the piece from (`r1`, `c1`) to (`r2`, `c2`),
        Returns whether the move is legal due to unblocked checks

        The state of the board is unchanged after the method call
        """
        old_state = _deepcopy(self.state)

        self.state[r2][c2] = self.state[r1][c1]  # Normal moves
        self.state[r1][c1] = '.'
        if self.state[r2][c2].upper() == 'P' and (r2, c2) == self.en_passant:  # En passant
            self.state[r1][c2] = '.'
        if self.state[r2][c2].upper() == 'K' and abs(c1 - c2) == 2:  # Castling
            self.state[r2][(c1 + c2) // 2] = 'R' if self.state[r2][c2].isupper() else 'r'
            if c2 == 2:
                self.state[r1][0] = '.'
            else:
                self.state[r1][7] = '.'

        legal = not self.check_check('b' if self.state[r2][c2].isupper() else 'w')
        self.state = old_state
        return legal

    def move(self, r1: int, c1: int, r2: int, c2: int) -> None:
        """
        Move the piece from (`r1`, `c1`) to (`r2`, `c2`)
        """
        self.state[r2][c2] = self.state[r1][c1]
        self.state[r1][c1] = '.'

        # Pawn promotion
        if (self.state[r2][c2] == 'p' and r2 == 7) or (self.state[r2][c2] == 'P' and r2 == 0):
            self.promotion = (r2, c2)

        # En passant
        if self.state[r2][c2].upper() == 'P' and (r2, c2) == self.en_passant:
            self.state[r1][c2] = '.'

        if self.state[r2][c2].upper() == 'P' and abs(r1 - r2) == 2:
            self.en_passant = ((r1 + r2) // 2, c1)
        else:
            self.en_passant = None

        # Castling
        if self.state[r2][c2].upper() == 'K' and abs(c1 - c2) == 2:
            self.state[r2][(c1 + c2) // 2] = 'R' if self.state[r2][c2].isupper() else 'r'
            if c2 == 2:
                self.state[r1][0] = '.'
            else:
                self.state[r1][7] = '.'

        if self.state[0][0] != 'r':
            self.castling = self.castling.replace('q', '')
        if self.state[0][7] != 'r':
            self.castling = self.castling.replace('k', '')
        if self.state[0][4] != 'k':
            self.castling = self.castling.replace('q', '')
            self.castling = self.castling.replace('k', '')
        if self.state[7][0] != 'R':
            self.castling = self.castling.replace('Q', '')
        if self.state[7][7] != 'R':
            self.castling = self.castling.replace('K', '')
        if self.state[7][4] != 'K':
            self.castling = self.castling.replace('Q', '')
            self.castling = self.castling.replace('K', '')

        # End turn
        self.turn = 'w' if self.turn == 'b' else 'b'

    def select(self, row: int, col: int) -> None:
        """
        Entry point for controller for normal moves.
        """
        if self.selected is not None:
            if (row, col) in self.get_piece_moves(self.selected[0], self.selected[1]):
                self.move(self.selected[0], self.selected[1], row, col)
            self.selected = None
            self.view.update([])
        else:
            color = 'w' if self.state[row][col].isupper() else 'b'
            if self.turn == color and len(self.get_piece_moves(row, col)) != 0:
                self.view.update(self.get_piece_moves(row, col))
                self.selected = (row, col)

    def promote(self, piece_id):
        """
        Entry point for controller to promote pieces.
        """
        print(piece_id)
        row, col = self.promotion
        color = 'w' if self.state[row][col].isupper() else 'b'
        self.state[row][col] = ['n', 'b', 'r', 'q'][piece_id]
        if color == 'w':
            self.state[row][col] = self.state[row][col].upper()
        self.promotion = None
        self.view.update([])