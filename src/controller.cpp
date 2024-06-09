#include "controller.h"
#include "SDL.h"

void Controller::HandleInput(bool &running, GameResources& game_resources) const {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      running = false;
    } else if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
        case SDLK_UP:
          game_resources.UpdateSnakeDirection(Snake::Direction::kUp);
          break;

        case SDLK_DOWN:
          game_resources.UpdateSnakeDirection(Snake::Direction::kDown);
          break;

        case SDLK_LEFT:
          game_resources.UpdateSnakeDirection(Snake::Direction::kLeft);
          break;

        case SDLK_RIGHT:
          game_resources.UpdateSnakeDirection(Snake::Direction::kRight);
          break;
      }
    }
  }
}