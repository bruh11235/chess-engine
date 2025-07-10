"""
Contains the ChessView class.
"""
import io
import pygame

WIDTH, HEIGHT = 512, 512

class ChessView:
    """
    Responsible for rendering the chessboard and pieces to the user interface.
    """
    def __init__(self):
        self.model = None
        self.screen = None

    def update(self, highlight):
        state = self.model.getState()

        screen_w, screen_h = self.screen.get_size()
        bg_img = pygame.transform.scale(pygame.image.load("resources/board.png"), (screen_w, screen_h))

        for r, c in highlight:
            red_filter = pygame.Surface((screen_w / 8, screen_h / 8), pygame.SRCALPHA)
            red_filter.fill((255, 0, 0, 127))
            bg_img.blit(red_filter, (screen_w / 8 * c, screen_h / 8 * r))

        self.screen.blit(bg_img, (0, 0))

        for r, row in enumerate(state):
            for c, char in enumerate(row):
                if char.isalpha():
                    path = "resources/pieces/" + ('w' if char.isupper() else 'b') + char.upper() + ".png"
                    piece = pygame.transform.scale(pygame.image.load(path), (screen_w / 8, screen_h / 8))
                    self.screen.blit(piece, (screen_w / 8 * c, screen_h / 8 * r))

        pygame.display.flip()
