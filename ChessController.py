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

        pygame.init()
        view.screen = pygame.display.set_mode((WIDTH, HEIGHT), pygame.RESIZABLE)

        view.update([])

        running = True
        while running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False

                elif event.type == pygame.MOUSEBUTTONUP:
                    x, y = pygame.mouse.get_pos()
                    model.select(int(y / HEIGHT * 8), int(x / WIDTH * 8))