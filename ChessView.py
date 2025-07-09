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

    def draw(self, screen):
        state: str = self.model.getState()

        screen_w, screen_h = screen.get_size()
        bg_img = pygame.transform.scale(pygame.image.load("resources/board.png"), (screen_w, screen_h))
        screen.blit(bg_img, (0, 0))

        state_ptr, empty_count = 0, 0
        for i in range(8):
            for j in range(8):
                print(state[state_ptr])

                if state[state_ptr].upper() in 'BKNPQR':
                    path = "resources/pieces/" + ('w' if state[state_ptr].isupper() else 'b') + state[state_ptr].upper() + ".png"
                    piece = pygame.transform.scale(pygame.image.load(path), (screen_w / 8, screen_h / 8))
                    screen.blit(piece, (screen_w / 8 * j, screen_h / 8 * i))

                if state[state_ptr].isnumeric():
                    empty_count += 1
                if (state[state_ptr].isnumeric() and empty_count >= int(state[state_ptr])) or state[state_ptr].isalpha():
                    state_ptr += 1
                    empty_count = 0
                while not state[state_ptr].isalnum():
                    state_ptr += 1

        pygame.display.flip()
