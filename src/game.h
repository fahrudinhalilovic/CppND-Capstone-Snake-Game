#ifndef GAME_H
#define GAME_H

#include <random>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "player.h"
#include "obstacles_generator.h"

class Game {
 public:
  Game(std::size_t grid_w, std::size_t grid_h, Player p);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;

 private:
  size_t grid_width;
  size_t grid_height;
  Player player;
  Snake snake;
  SDL_Point food;
  std::vector<SDL_Point> obstacles;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};

  void PlaceFood();
  void PlaceObstacle(const ObstaclesGenerator::SPtr& generator);
  bool CanPlaceObstacle(const SDL_Point& point);
  void Update(Level lvl, const ObstaclesGenerator::SPtr& generator);
};

#endif