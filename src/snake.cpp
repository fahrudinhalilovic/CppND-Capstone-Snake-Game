#include "snake.h"
#include "snake_hunter.h"

#include <cmath>
#include <iostream>

void Snake::Update(const std::vector<SDL_Point>& obstacles, const SnakeHunter& snake_hunter) {
  // We first capture the head's cell before updating.
  SDL_Point prev_cell {
      static_cast<int>(head_x),
      static_cast<int>(head_y)
  };

  UpdateHead();

  // Capture the head's cell after updating.
  SDL_Point current_cell {
      static_cast<int>(head_x),
      static_cast<int>(head_y)};

  // Update all of the body vector items if the snake head has moved to a new cell.
  if (current_cell.x != prev_cell.x || current_cell.y != prev_cell.y) {
    UpdateBody(current_cell, prev_cell, obstacles, snake_hunter);
  }
}

void Snake::UpdateHead() {
  switch (direction) {
    case Direction::kUp:
      head_y -= speed;
      break;

    case Direction::kDown:
      head_y += speed;
      break;

    case Direction::kLeft:
      head_x -= speed;
      break;

    case Direction::kRight:
      head_x += speed;
      break;
  }

  // Wrap the Snake around to the beginning if going off of the screen.
  head_x = fmod(head_x + grid_width, grid_width);
  head_y = fmod(head_y + grid_height, grid_height);
}

void Snake::UpdateBody(SDL_Point &current_head_cell, SDL_Point &prev_head_cell,
                      const std::vector<SDL_Point>& obstacles, const SnakeHunter& snake_hunter) {
  // Add previous head location to vector
  body.push_back(prev_head_cell);

  if (!growing) {
    // Remove the tail from the vector.
    body.erase(body.begin());
  } else {
    growing = false;
    size++;
  }

  // Check if the snake has died.
  for (auto const &item : body) {
    if (current_head_cell.x == item.x && current_head_cell.y == item.y) {
      alive = false;
      death_cause = DeathCause::SelfMurder;
    }
  }
  // Check if the snake hit the obstacle.
  for(const auto& o : obstacles) {
    if ( current_head_cell.x == o.x && current_head_cell.y == o.y ) {
      alive = false;
      death_cause = DeathCause::HitTheObstacle;
    }
  }
  // Check if the snake hunter catched the snake.
  if ( snake_hunter.SnakeHunterCell(head_x, head_y) ) {
    alive = false;
    death_cause = DeathCause::CatchedBySnakeHunter;
  }
  for( const auto& item : body ) {
    if ( snake_hunter.SnakeHunterCell(item.x, item.y) ) {
      alive = false;
      death_cause = DeathCause::CatchedBySnakeHunter;
    }
  }
}

void Snake::GrowBody() { growing = true; }

// Inefficient method to check if cell is occupied by snake.
bool Snake::SnakeCell(int x, int y) {
  if (x == static_cast<int>(head_x) && y == static_cast<int>(head_y)) {
    return true;
  }
  for (auto const &item : body) {
    if (x == item.x && y == item.y) {
      return true;
    }
  }
  return false;
}

DeathCause Snake::FindDeathCause() const {
  return death_cause;
}