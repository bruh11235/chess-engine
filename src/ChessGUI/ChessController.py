"""
Contains the ChessController class.
"""
from __future__ import annotations
import pygame

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from ChessModel import ChessModel
    from ChessView import ChessView


WIDTH, HEIGHT = 512, 512


class ChessController:
    """
    Manages user input and controls the flow of the chess game.
    """

    def __init__(self, model: ChessModel, view: ChessView):
        self.model = model
        self.view = view

        model.view = view
        view.model = model

    def start(self):
        pygame.init()
        self.view.screen = pygame.display.set_mode((WIDTH, HEIGHT))
        self.view.update([])

        running = True
        while running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False

                elif event.type == pygame.MOUSEBUTTONUP:
                    x, y = pygame.mouse.get_pos()
                    if self.model.promotion is None:
                        row, col = int(y / HEIGHT * 8), int(x / WIDTH * 8)
                        if 0 <= row < 8 and 0 <= col < 8:
                            self.model.select(int(y / HEIGHT * 8), int(x / WIDTH * 8))
                    else:
                        for i, (button_x, button_y, button_w, button_h) in enumerate(self.view.promotion_buttons):
                            if button_x <= x <= button_x + button_w and button_y <= y <= button_y + button_h:
                                self.model.promote(i)
