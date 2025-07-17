"""
Contains the ChessView class.
"""
from __future__ import annotations
import pygame

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from src.ChessGUI.ChessModel import ChessModel


class ChessView:
    """
    Responsible for rendering the chessboard and pieces to the user interface.
    """
    def __init__(self):
        self.model: ChessModel | None = None
        self.screen: pygame.Surface | pygame.SurfaceType | None = None
        self.promotion_buttons: list = []

    def update(self, highlight: list):
        """
        Display the current state of the game, with `highlight`ed squares having a red shade
        """
        state = self.model.get_state()

        screen_w, screen_h = self.screen.get_size()
        bg_img = pygame.transform.scale(pygame.image.load("resources/board.png"), (screen_w, screen_h))

        for r, c in highlight:
            red_filter = pygame.Surface((screen_w // 8, screen_h // 8), pygame.SRCALPHA)
            red_filter.fill((255, 0, 0, 127))
            bg_img.blit(red_filter, (screen_w // 8 * c, screen_h // 8 * r))

        self.screen.blit(bg_img, (0, 0))

        for r, row in enumerate(state):
            for c, char in enumerate(row):
                if char.isalpha():
                    path = "resources/pieces/" + ('w' if char.isupper() else 'b') + char.upper() + ".png"
                    piece = pygame.transform.scale(pygame.image.load(path), (screen_w // 8, screen_h // 8))
                    self.screen.blit(piece, (screen_w // 8 * c, screen_h // 8 * r))

        if self.model.promotion is not None:
            self.promotion_popup()

        pygame.display.flip()

    def promotion_popup(self):
        """
        Draw a popup for choosing promotions.
        """
        screen_w, screen_h = self.screen.get_size()
        button_width, button_height = screen_w // 8, screen_h // 8
        popup_width, popup_height = button_width * 4 + 5 * 5, button_height + 5 * 2
        popup_x, popup_y = (screen_w - popup_width) // 2, (screen_h - popup_height) // 2

        pygame.draw.rect(self.screen, (200, 200, 200), (popup_x, popup_y, popup_width, popup_height))
        pygame.draw.rect(self.screen, (0, 0, 0), (popup_x, popup_y, popup_width, popup_height), 2)

        self.promotion_buttons = []
        for offset, icon in [(0, "wN"), (1, "wB"), (2, "wR"), (3, "wQ")]:
            button_x, button_y = popup_x + (button_width + 5) * offset + 5, popup_y + 5
            button_rect = pygame.Rect(button_x, button_y, button_width, button_height)
            pygame.draw.rect(self.screen, (255, 255, 255), button_rect)
            pygame.draw.rect(self.screen, (0, 0, 0), button_rect, 2)

            path = "resources/pieces/" + icon + ".png"
            piece = pygame.transform.scale(pygame.image.load(path), (screen_w / 8, screen_h / 8))
            self.screen.blit(piece, (button_x, button_y))
            self.promotion_buttons.append((button_x, button_y, button_width, button_height))
