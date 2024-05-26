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
      player { std::move(p) },
      snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  PlaceFood();
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

    std::unique_lock obstacles_lock { obstacles_mutex };
    std::unique_lock food_lock { food_mutex };
    std::unique_lock snake_lock { snake_mutex };
    renderer.Render(snake, food, obstacles);
    obstacles_lock.unlock();
    food_lock.unlock();
    snake_lock.unlock();

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

  // game stopped, so stop the thread for creating obstacles too
  obstacles_thread.join();

  // save potentially new highest score to the file
  player.PersistHighestScoresToFile(lvl);
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by some of the obstacles or by the snake
    if ( !IsOccupiedByObstacles(SDL_Point {x, y}) && !snake.SnakeCell(x, y) ) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

void Game::PlaceObstacle(ObstaclesGenerator::SPtr generator)
{
  while ( true ) {
    // pause for 4s before creating a new obstacle
    SDL_Delay(4000u);

    std::unique_lock running_flag_lock { running_flag_mutex };
    if ( !running ) {
      return;
    }
    running_flag_lock.unlock();

    std::unique_lock snake_lock { snake_mutex };
    if ( !snake.alive ) {
      return;
    }
    snake_lock.unlock();

    auto cnt = 0u;
    auto obstacle_generated = true;
    auto obstacle = generator->CreateObstacle();

    if ( !obstacle ) {
      // for beginner level we do not even generate obstacles...
      obstacle_generated = false;
    }

    while ( obstacle_generated && !CanPlaceObstacle(*obstacle) ) {
      if ( ++cnt >= 10u ) {
        // if we need more than 10 tries to get new obstacle
        // then density of the obstacles on the playground is
        // probably too high - thus we skip generating now
        // and try once again after certain time period
        obstacle_generated = false;
      }
      obstacle = generator->CreateObstacle();
    }

    if ( obstacle_generated ) {
      std::lock_guard obstacles_lock { obstacles_mutex };
      obstacles.emplace_back(std::move(*obstacle));
    }
  }
}

bool Game::IsOccupiedByObstacles(const SDL_Point& point)
{
    std::lock_guard obstacles_lock { obstacles_mutex };

    // check if another obstacle occupied this place
    for(const auto& o : obstacles) {
      if ( o.x == point.x && o.y == point.y ) {
        return true;
      }
    }

    return false;
}

bool Game::CanPlaceObstacle(const SDL_Point& point)
{
    std::unique_lock food_lock { food_mutex };
    // check if food occupied this place
    if ( food.x == point.x && food.y == point.y ) {
      return false;
    }
    food_lock.unlock();

    std::unique_lock snake_lock { snake_mutex };
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

    // end of section for checking snake - unlock it now
    snake_lock.unlock();

    if ( IsOccupiedByObstacles(point) ) {
      return false;
    }

    // otherwise, this place is free so put the new obstacle here
    return true;
}

void Game::Update(Level lvl) {
  // snake is used through entire method so lock it
  // until the end of the method...
  std::lock_guard snake_lock { snake_mutex };

  if (!snake.alive) return;

  std::unique_lock obstacles_lock { obstacles_mutex };
  snake.Update(obstacles);
  obstacles_lock.unlock();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  std::lock_guard food_lock { food_mutex };
  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    player.UpdateHighestScore(lvl, ++score);
    PlaceFood();
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.02;
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() {
  std::lock_guard snake_lock { snake_mutex };
  return snake.size;
}
