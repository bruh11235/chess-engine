"""
The main chess program.
"""
from ChessController import ChessController
from ChessView import ChessView
from ChessModel import ChessModel

c = ChessController(ChessModel(), ChessView())