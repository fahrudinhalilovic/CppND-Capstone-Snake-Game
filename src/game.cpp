#include "game.h"
#include "obstacles_generator.h"

#include <iostream>
#include <regex>

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
      player { std::move(p) },
      snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  PlaceFood();
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  auto lvl = InputGameLevel();
  auto obstacles_generator = ObstaclesGenerator::CreateObstaclesGenerator(lvl, grid_width, grid_height,snake);

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    Update(lvl, obstacles_generator);
    renderer.Render(snake, food, obstacles);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(player.Username(), score, frame_count);
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

  // save potentially new highest score to the file
  player.PersistHighestScore();
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(x, y)) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

void Game::PlaceObstacle(const ObstaclesGenerator::SPtr& generator)
{
  auto obstacle = generator->CreateObstacle();
  if ( !obstacle ) {
    // for beginner level we do not even generate obstacles...
    return;
  }

  while ( !CanPlaceObstacle(obstacle.value()) ) {
    obstacle = generator->CreateObstacle();
  }

  obstacles.emplace_back(std::move(*obstacle));
}

bool Game::CanPlaceObstacle(const SDL_Point& point)
{
    // check if food occupied this place
    if ( food.x == point.x && food.y == point.y ) {
      return false;
    }

    // check if snake occupied this place
    if ( snake.SnakeCell(point.x, point.y) ) {
      return false;
    }

    // check if snake is too close to this field
    auto x_diff = static_cast<size_t>(std::abs(snake.head_x - point.x));
    auto y_diff = static_cast<size_t>(std::abs(snake.head_y - point.y));
    auto total_diff = x_diff + y_diff;
    // the newset obstacle should always be at least 3 fields
    // away from the snake to give player some time to react
    // on the newly positioned obstacle
    if ( total_diff <= 3 ) {
      return false;
    }

    // check if another obstacle occupied this place
    for(const auto& o : obstacles) {
      if ( o.x == point.x && o.y == point.y ) {
        return false;
      }
    }

    // otherwise, this place is free so put new obstacle here
    return true;
}

void Game::Update(Level lvl, const ObstaclesGenerator::SPtr& generator) {
  if (!snake.alive) return;

  snake.Update(obstacles);

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    player.UpdateHighestScore(lvl, ++score);
    PlaceFood();
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.02;
  }

  static Uint32 last_update = SDL_GetTicks();
  Uint32 curr = SDL_GetTicks();
  // generate new obstacle every 4sec...
  if ( curr - last_update > 4000 ) {
    last_update = SDL_GetTicks();
    PlaceObstacle(generator);
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }
