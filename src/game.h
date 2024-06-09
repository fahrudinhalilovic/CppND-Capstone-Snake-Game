#ifndef GAME_H
#define GAME_H

#include <random>
#include <mutex>

#include "SDL.h"

#include "controller.h"
#include "renderer.h"
#include "game_resources.h"

Level InputGameLevel();

class Game {
 public:
  Game(std::size_t grid_w, std::size_t grid_h, Player p, Level lvl);
  void Run(Controller const &controller, Renderer &renderer, std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize();

private:
  void Update();
  void PlaceObstacle();

 private:
  bool running = true;
  size_t grid_width;
  size_t grid_height;
  GameResources::UPtr game_resources;
  std::mutex running_flag_mutex;
  int score{0};
};

#endif