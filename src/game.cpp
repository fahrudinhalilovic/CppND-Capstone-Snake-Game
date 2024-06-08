#include "game.h"
#include "obstacles_generator.h"

#include <iostream>
#include <regex>
#include <thread>

#include "SDL.h"

Level InputGameLevel()
{
    std::cout << "Please provide level of the game (1=beginner, 2=medium, 3=advanced):" << std::endl;
    std::regex regex { "[1-3]" };
    std::string input;

    while( std::getline(std::cin, input) ) {
        if ( std::regex_match(input, regex) ) {
            return FromInputString(input);
        }
        else {
          std::cerr << "Please provide one of the following: 1, 2 or 3!" << std::endl;
        }
    }

    return Level::Medium;
}

Game::Game(std::size_t grid_w, std::size_t grid_h, Player p)
    : grid_width { grid_w },
      grid_height { grid_h },
      game_resources { std::make_unique<GameResources>(grid_w, grid_h, std::move(p)) } {
  game_resources->PlaceFood();
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration, Level lvl) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;

  auto obstacles_generator = ObstaclesGenerator::ObstaclesFactory(lvl, grid_width, grid_height, snake);

  std::thread obstacles_thread { &Game::PlaceObstacle, this, std::move(obstacles_generator) };

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.

    std::unique_lock running_flag_lock { running_flag_mutex };
    controller.HandleInput(running, snake);
    running_flag_lock.unlock();

    Update(lvl);

    game_resources->Hunt();

    renderer.Render(snake, food, obstacles, snake_hunter);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(game_resources->PlayersUsername(), score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }

  // game stopped, so stop the thread for creating obstacles too
  obstacles_thread.join();
}

void Game::PlaceObstacle(ObstaclesGenerator::SPtr generator)
{
  Uint32 obstacle_generated = SDL_GetTicks();

  while ( true ) {

    std::unique_lock running_flag_lock { running_flag_mutex };
    if ( !running ) {
      return;
    }
    running_flag_lock.unlock();

    Uint32 curr_ticks = SDL_GetTicks();
    // 4s delay before creating a new obstacle
    if ( curr_ticks - obstacle_generated < 4000u ) {
      // make a small break to avoid killing CPU
      SDL_Delay(100u);
      continue;
    }

    obstacle_generated = curr_ticks;

    if ( game_resources->GameFinished() ) {
      return;
    }

    // TODO: check this generator thingy - this should not be passed like this...
    game_resources->PlaceObstacle(generator);
  }
}

void Game::Update(Level lvl) {

  if ( game_resources->GameFinished() ) return;

  if ( game_resources->UpdateSnake(lvl) ) {
    ++score;
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() { return game_resources->SnakeSize(); }
