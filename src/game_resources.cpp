#include "game_resources.h"
#include "obstacles_generator.h"

#include "SDL.h"

GameResources::GameResources(std::size_t grid_w, std::size_t grid_h, Player p, Level lvl)
    : grid_width { grid_w },
      grid_height { grid_h },
      player { std::move(p) },
      level { lvl },
      snake(grid_width, grid_height),
      snake_hunter { grid_width, grid_height },
      obstacles_generator { ObstaclesGenerator::ObstaclesFactory(level, grid_width, grid_height, snake) },
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {}

GameResources::~GameResources()
{
  // save potentially new highest score to the file
  player.PersistHighestScoresToFile(level);
}

bool GameResources::GameFinished()
{
  std::lock_guard resources_lock { resources_mutex };

  return GameFinishedImpl();
}

void GameResources::UpdatePlayersHighestScore(int new_score)
{
  std::lock_guard resources_lock { resources_mutex };

  player.UpdateHighestScore(level, new_score);
}

void GameResources::PlaceFood() {
  std::lock_guard resources_lock { resources_mutex };

  PlaceFoodImpl();
}

void GameResources::PlaceObstacle()
{
  std::lock_guard resources_lock { resources_mutex };

  auto cnt = 0u;
  auto obstacle_generated = true;
  auto obstacle = obstacles_generator->CreateObstacle();

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
    obstacle = obstacles_generator->CreateObstacle();
  }

  if ( obstacle_generated ) {
    obstacles.emplace_back(std::move(*obstacle));
  }
}

int GameResources::SnakeSize()
{
  std::lock_guard resources_lock { resources_mutex };

  return snake.size;
}

bool GameResources::UpdateSnake()
{
  std::lock_guard resources_lock { resources_mutex };

  if ( GameFinishedImpl() ) {
    return false;
  }

  snake.Update(obstacles, snake_hunter);


  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  bool found_food = false;

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    found_food = true;
    PlaceFoodImpl();
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.02;
  }

  return found_food;
}

Snake::Direction GetOppositeDirection(Snake::Direction direction)
{
  switch ( direction ) {
    case Snake::Direction::kUp:
      return Snake::Direction::kDown;
    case Snake::Direction::kDown:
      return Snake::Direction::kUp;
    case Snake::Direction::kLeft:
      return Snake::Direction::kRight;
    case Snake::Direction::kRight:
      return Snake::Direction::kLeft;
  }
}

void GameResources::UpdateSnakeDirection(Snake::Direction input)
{
  std::lock_guard resources_lock { resources_mutex };

  Snake::Direction opposite = GetOppositeDirection(input);

  if ( snake.direction != opposite || snake.size == 1 ) {
    snake.direction = input;
  }
}

void GameResources::Hunt()
{
  std::lock_guard resources_lock { resources_mutex };

    if ( GameFinishedImpl() ) {
        return;
    }

  snake_hunter.Hunt(snake, obstacles, food);
}

bool GameResources::GameFinishedImpl()
{
  return !snake.alive;
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
