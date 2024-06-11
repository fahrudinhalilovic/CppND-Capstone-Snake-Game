#include "renderer.h"
#include <iostream>
#include <sstream>
#include <string>

Renderer::Renderer(const std::size_t screen_width,
                   const std::size_t screen_height,
                   const std::size_t grid_width,
                   const std::size_t grid_height)
    : screen_width(screen_width),
      screen_height(screen_height),
      grid_width(grid_width),
      grid_height(grid_height) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create Window
  sdl_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, screen_width,
                                screen_height, SDL_WINDOW_SHOWN);

  if (nullptr == sdl_window) {
    std::cerr << "Window could not be created.\n";
    std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create renderer
  sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == sdl_renderer) {
    std::cerr << "Renderer could not be created.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }
}

Renderer::~Renderer() {
  SDL_DestroyWindow(sdl_window);
  SDL_Quit();
}

void Renderer::Render(GameResources& game_resources) {
  std::lock_guard resources_lock { game_resources.resources_mutex };

  const Snake& snake = game_resources.snake;
  const SDL_Point& food = game_resources.food;
  const std::vector<SDL_Point>& obstacles = game_resources.obstacles;
  const SnakeHunter& snake_hunter = game_resources.snake_hunter;

  SDL_Rect block;
  block.w = screen_width / grid_width;
  block.h = screen_height / grid_height;

  // Clear screen
  SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(sdl_renderer);

  // Render food
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xCC, 0x00, 0xFF);
  block.x = food.x * block.w;
  block.y = food.y * block.h;
  SDL_RenderFillRect(sdl_renderer, &block);

  // Render obstacles
  SDL_SetRenderDrawColor(sdl_renderer, 0x80, 0x80, 0x80, 0xFF);
  for(const auto& o : obstacles) {
    block.x = o.x * block.w;
    block.y = o.y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Render snake's body
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  for (SDL_Point const &point : snake.body) {
    block.x = point.x * block.w;
    block.y = point.y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Render snake's head
  block.x = static_cast<int>(snake.head_x) * block.w;
  block.y = static_cast<int>(snake.head_y) * block.h;
  SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0x7A, 0xCC, 0xFF);
  SDL_RenderFillRect(sdl_renderer, &block);

  // Render snake hunter
  SDL_SetRenderDrawColor(sdl_renderer, 255, 87, 51, 0xFF);
  auto snake_hunter_pt = snake_hunter.CurrentPosition();
  block.x = snake_hunter_pt.x * block.w;
  block.y = snake_hunter_pt.y * block.h;
  SDL_RenderFillRect(sdl_renderer, &block);

  // If game has finished, then render special red rectangle
  // on the problematic field.
  if ( game_resources.GameFinishedImpl() ) {
    // Red color when snake dies.
    SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);

    switch ( snake.FindDeathCause() ) {
      case DeathCause::SelfMurder:
      case DeathCause::HitTheObstacle:
        block.x = static_cast<int>(snake.head_x) * block.w;
        block.y = static_cast<int>(snake.head_y) * block.h;
        break;
      case DeathCause::CatchedBySnakeHunter:
        block.x = snake_hunter_pt.x * block.w;
        block.y = snake_hunter_pt.y * block.h;
        break;
    }

    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Update Screen
  SDL_RenderPresent(sdl_renderer);
}

void Renderer::UpdateWindowTitle(GameResources& game_resources, int score, int fps) {
  std::lock_guard resources_lock { game_resources.resources_mutex };

  const auto lvl = game_resources.level;
  std::ostringstream oss;

  oss << "Player: " + game_resources.player.Username() << " ";
  oss << "Highest score (" << ToString(lvl) << "): " <<  game_resources.player.HighestScore(lvl) << " ";
  oss << "Snake Score: " + std::to_string(score) << " ";
  oss << "FPS: " + std::to_string(fps);

  const auto title = oss.str();
  SDL_SetWindowTitle(sdl_window, title.c_str());
}
