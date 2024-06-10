#ifndef GAME_RESOURCES_H
#define GAME_RESOURCES_H

#include <random>
#include <mutex>

#include "SDL.h"

#include "snake.h"
#include "snake_hunter.h"
#include "player.h"
#include "obstacles_generator.h"

class Renderer;

class GameResources {
 public:
  using UPtr = std::unique_ptr<GameResources>;

  friend class Renderer;

  GameResources(std::size_t grid_w, std::size_t grid_h, Player p, Level lvl);
  ~GameResources();

  bool GameFinished();
  void UpdatePlayersHighestScore(int new_score);
  void PlaceFood();
  void PlaceObstacle();
  int SnakeSize();
  bool UpdateSnake();
  void UpdateSnakeDirection(Snake::Direction input);
  void Hunt();

private:

  bool GameFinishedImpl();
  void PlaceFoodImpl();
  bool IsOccupiedByObstacles(int x, int y);
  bool CanPlaceObstacle(int x, int y);

 private:
  size_t grid_width;
  size_t grid_height;

  Player player;
  Level level;
  Snake snake;
  SnakeHunter snake_hunter;
  ObstaclesGenerator::SPtr obstacles_generator;
  std::vector<SDL_Point> obstacles;
  SDL_Point food;

  std::mutex resources_mutex;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;
};

#endif
