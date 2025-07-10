"""
Contains the ChessModel class.
"""

KNIGHT_MOVES = ((1, 2), (2, 1), (-1, 2), (-2, 1), (1, -2), (2, -1), (-1, -2), (-2, -1))
DIR8 = ((1, 0), (-1, 0), (0, 1), (0, -1), (1, 1), (1, -1), (-1, 1), (-1, -1))


class ChessModel:
    """
    Represents the data and game logic layer.
    """

    def __init__(self):
        self.view = None
        self.turn = 'w'
        self.castling = 'KQkq'
        self.en_passant = ''
        self.half_move = 0
        self.full_move = 0
        self.state = [['r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'],
                      ['p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'],  # Black
                      ['.', '.', '.', '.', '.', '.', '.', '.'],
                      ['.', '.', '.', '.', '.', '.', '.', '.'],
                      ['.', '.', '.', '.', '.', '.', '.', '.'],
                      ['.', '.', '.', '.', '.', '.', '.', '.'],
                      ['P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'],
                      ['R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R']]  # White
        self.selected = None

    def get_piece_moves(self, row, col) -> list:
        if not(0 <= row < 8 and 0 <= col < 8) or self.state[row][col] == '.':
            return []

        color = 'w' if self.state[row][col].isupper() else 'b'
        moves = []

        # ===== Pawn =====
        # TODO: en passant & Pawn promotion
        if self.state[row][col].upper() == 'P':
            at_home = (color == 'w' and row == 6) or (color == 'b' and row == 1)
            direction = 1 if color == 'b' else -1

            if self.state[row + direction][col] == '.':  # Move forwards
                moves.append((row + direction, col))
                if at_home and self.state[row + direction * 2][col] == '.':
                    moves.append((row + direction * 2, col))

            for side in [-1, 1]:  # Capture
                if 0 <= col + side < 8 and self.state[row + direction][col + side].isalpha():
                    other_color = 'w' if self.state[row + direction][col + side].isupper() else 'b'
                    if color != other_color:
                        moves.append((row + direction, col + side))

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
                            moves.append((cx, cy))
                        break
                    moves.append((cx, cy))

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
                            moves.append((cx, cy))
                        break
                    moves.append((cx, cy))

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
                            moves.append((cx, cy))
                        break
                    moves.append((cx, cy))

        # ===== Knight =====
        elif self.state[row][col].upper() == 'N':
            for dx, dy in KNIGHT_MOVES:
                if not(0 <= row + dx < 8 and 0 <= col + dy < 8):
                    continue
                other_color = 'w' if self.state[row + dx][col + dy].isupper() else 'b'
                if self.state[row + dx][col + dy] == '.' or other_color != color:
                    moves.append((row + dx, col + dy))

        # ===== King =====
        # TODO: Castling
        elif self.state[row][col].upper() == 'K':
            for dx, dy in DIR8:
                if not(0 <= row + dx < 8 and 0 <= col + dy < 8):
                    continue
                other_color = 'w' if self.state[row + dx][col + dy].isupper() else 'b'
                if self.state[row + dx][col + dy] == '.' or other_color != color:
                    moves.append((row + dx, col + dy))

        return moves

    def get_moves(self) -> list:
        pass

    def getState(self):
        return self.state

    def select(self, row, col):
        if self.selected is not None:
            if (row, col) in self.get_piece_moves(self.selected[0], self.selected[1]):
                self.state[row][col] = self.state[self.selected[0]][self.selected[1]]
                self.state[self.selected[0]][self.selected[1]] = '.'
            self.selected = None
            self.view.update([])
        else:
            self.view.update(self.get_piece_moves(row, col))
            if len(self.get_piece_moves(row, col)) != 0:
                self.selected = (row, col)