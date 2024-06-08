#include "game_resources.h"
#include "obstacles_generator.h"

#include <iostream>
#include <regex>
#include <thread>

#include "SDL.h"

GameResources::GameResources(std::size_t grid_w, std::size_t grid_h, Player p)
    : grid_width { grid_w },
      grid_height { grid_h },
      player { std::move(p) },
      snake(grid_width, grid_height),
      snake_hunter { grid_width, grid_height },
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {}

GameResources::~GameResources()
{
  // TODO: fix this ...
  
  // save potentially new highest score to the file
  // player.PersistHighestScoresToFile(lvl);
}

bool GameResources::GameFinished()
{
  std::lock_guard resources_lock { resources_mutex };

  return !snake.alive;
}

int GameResources::SnakeSize()
{
  std::lock_guard resources_lock { resources_mutex };

  return snake.size;
}

std::string GameResources::PlayersUsername()
{
  std::lock_guard resources_lock { resources_mutex };

  return player.Username();
}

void GameResources::PlaceFood() {
  std::lock_guard resources_lock { resources_mutex };

  PlaceFoodImpl();
}

void GameResources::PlaceObstacle(ObstaclesGenerator::SPtr generator)
{
  std::lock_guard resources_lock { resources_mutex };

  auto cnt = 0u;
  auto obstacle_generated = true;
  auto obstacle = generator->CreateObstacle();

  if ( !obstacle ) {
    // for beginner level we do not even generate obstacles...
    obstacle_generated = false;
  }

  while ( obstacle_generated && !CanPlaceObstacle(obstacle->x, obstacle->y) ) {
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
    obstacles.emplace_back(std::move(*obstacle));
  }
}

bool GameResources::UpdateSnake(Level lvl)
{
  std::lock_guard resources_lock { resources_mutex };

  if (!snake.alive) return;

  snake.Update(obstacles, snake_hunter);


  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  bool found_food = false;

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    found_food = true;
    player.UpdateHighestScore(lvl, ++score);
    PlaceFoodImpl();
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.02;
  }

  return found_food;
}

void GameResources::Hunt()
{
  std::lock_guard resources_lock { resources_mutex };

  snake_hunter.Hunt(snake, obstacles, food);
}

void GameResources::PlaceFoodImpl()
{
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by some of the obstacles or by the snake
    if ( !IsOccupiedByObstacles(x, y) && !snake.SnakeCell(x, y) && !snake_hunter.SnakeHunterCell(x, y) ) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

bool GameResources::IsOccupiedByObstacles(int x, int y)
{
    // check if another obstacle occupied this place
    for(const auto& o : obstacles) {
      if ( o.x == x && o.y == y ) {
        return true;
      }
    }

    return false;
}

bool GameResources::CanPlaceObstacle(int x, int y)
{
    // check if food occupied this place
    if ( food.x == x && food.y == y ) {
      return false;
    }

    // check if snake occupied this place
    if ( snake.SnakeCell(x, y) ) {
      return false;
    }

    // check if snake's head is too close to this field
    auto x_diff = static_cast<size_t>(std::abs(snake.head_x - x));
    auto y_diff = static_cast<size_t>(std::abs(snake.head_y - y));
    auto total_diff = x_diff + y_diff;
    // the newset obstacle should always be at least 3 fields
    // away from the snake to give player some time to react
    // on the newly positioned obstacle
    if ( total_diff <= 3 ) {
      return false;
    }

    if ( IsOccupiedByObstacles(x, y) ) {
      return false;
    }

    // check if the snake hunter is located here
    if ( snake_hunter.SnakeHunterCell(x, y) ) {
      return false;
    }

    // otherwise, this place is free so put the new obstacle here
    return true;
}
