#ifndef GAME_H
#define GAME_H

#include <random>
#include <mutex>

#include "SDL.h"

#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "player.h"
#include "obstacles_generator.h"
#include "obstacles_generators/snake_hunter.h"

Level InputGameLevel();

class Game {
 public:
  Game(std::size_t grid_w, std::size_t grid_h, Player p);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration, Level lvl);
  int GetScore() const;
  int GetSize();

 private:
  bool running = true;
  size_t grid_width;
  size_t grid_height;
  Player player;
  Snake snake;
  SDL_Point food;
  SnakeHunter snake_hunter;
  std::vector<SDL_Point> obstacles;
  std::mutex running_flag_mutex;
  std::mutex obstacles_mutex;
  std::mutex snake_mutex;
  std::mutex food_mutex;
  std::mutex snake_hunter_mutex;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};

  void PlaceFood();
  void PlaceObstacle(ObstaclesGenerator::SPtr generator);
  bool IsOccupiedByObstacles(const SDL_Point& point);
  bool CanPlaceObstacle(const SDL_Point& point);
  void Update(Level lvl);
};

#endif