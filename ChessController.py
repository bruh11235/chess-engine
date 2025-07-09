"""
Contains the ChessController class.
"""
import pygame

WIDTH, HEIGHT = 512, 512

class ChessController:
    """
    Manages user input and controls the flow of the chess game.
    """

    def __init__(self, model, view):
        model.view = view
        view.model = model
        self.model = model
        self.view = view

        pygame.init()
        pygame.font.init()
        pygame.mixer.init()
        screen = pygame.display.set_mode((WIDTH, HEIGHT), pygame.RESIZABLE)

        self.view.draw(screen)

        running = True
        while running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False