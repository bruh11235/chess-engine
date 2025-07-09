"""
Contains the ChessModel class.
"""

class ChessModel:
    """
    Represents the data and game logic layer.
    """

    def __init__(self):
        self.view = None
        self.state = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1' # Forsyth-Edwards Notation

    def getState(self):
        return self.state