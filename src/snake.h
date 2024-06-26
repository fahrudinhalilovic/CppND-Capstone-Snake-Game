#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include "SDL.h"

class SnakeHunter;

enum class DeathCause {
  SelfMurder,
  HitTheObstacle,
  CatchedBySnakeHunter
};

class Snake {
 public:
  enum class Direction { kUp, kDown, kLeft, kRight };

  Snake(int grid_width, int grid_height)
      : grid_width(grid_width),
        grid_height(grid_height),
        head_x(grid_width / 2),
        head_y(grid_height / 2) {}

  void Update(const std::vector<SDL_Point>& obstacles, const SnakeHunter& snake_hunter);

  void GrowBody();
  bool SnakeCell(int x, int y);
  DeathCause FindDeathCause() const;

  Direction direction = Direction::kUp;

  float speed{0.1f};
  int size{1};
  bool alive{true};
  float head_x;
  float head_y;
  std::vector<SDL_Point> body;

 private:
  void UpdateHead();
  void UpdateBody(SDL_Point &current_cell, SDL_Point &prev_cell, const std::vector<SDL_Point>& obstacles,
                  const SnakeHunter& snake_hunter);

  bool growing{false};
  int grid_width;
  int grid_height;
  DeathCause death_cause;
};

#endif