#ifndef GAME_RESOURCES_H
#define GAME_RESOURCES_H

#include <random>
#include <mutex>

#include "SDL.h"

#include "snake.h"
#include "snake_hunter.h"
#include "player.h"
#include "obstacles_generator.h"

class GameResources {
 public:
  using UPtr = std::unique_ptr<GameResources>;

  GameResources(std::size_t grid_w, std::size_t grid_h, Player p);
  ~GameResources();

  bool GameFinished();
  int SnakeSize();
  std::string PlayersUsername();
  void PlaceFood();
  void PlaceObstacle(ObstaclesGenerator::SPtr generator);
  bool UpdateSnake(Level lvl);
  void Hunt();

private:

  void PlaceFoodImpl();
  bool IsOccupiedByObstacles(int x, int y);
  bool CanPlaceObstacle(int x, int y);

 private:
  size_t grid_width;
  size_t grid_height;

  Player player;
  Snake snake;
  SnakeHunter snake_hunter;
  SDL_Point food;
  std::vector<SDL_Point> obstacles;

  std::mutex resources_mutex;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};
};

#endif
