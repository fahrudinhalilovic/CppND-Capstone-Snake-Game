#include "advanced.h"

#include "../snake.h"

#include <iostream>

AdvancedObstaclesGenerator::AdvancedObstaclesGenerator(size_t grid_w, size_t grid_h, const Snake& s)
    : grid_width { grid_w },
      grid_height { grid_h },
      snake { s } { }

std::optional<SDL_Point> AdvancedObstaclesGenerator::CreateObstacle() const
{
    constexpr size_t offset = 3;

    int x_min = snake.head_x;
    int x_max = snake.head_x;
    int y_min = snake.head_y;
    int y_max = snake.head_y;

    switch ( snake.direction ) {
        case Snake::Direction::kUp:
            x_min = snake.head_x - 1;
            x_max = snake.head_x + 1;
            y_max = snake.head_y - offset - 1;
            y_min = snake.head_y - offset - 3;
            break;
        case Snake::Direction::kDown:
            x_min = snake.head_x - 1;
            x_max = snake.head_x + 1;
            y_min = snake.head_y + offset + 1;
            y_max = snake.head_y + offset + 3;
            break;
        case Snake::Direction::kLeft:
            x_max = snake.head_x - offset - 1;
            x_min = snake.head_x - offset - 3;
            y_min = snake.head_y - 1;
            y_max = snake.head_y + 1;
            break;
        case Snake::Direction::kRight:
            x_min = snake.head_x + offset + 1;
            x_max = snake.head_x + offset + 3;
            y_min = snake.head_y - 1;
            y_max = snake.head_y + 1;
            break;
    }

    std::uniform_int_distribution<int> random_w { x_min, x_max };
    std::uniform_int_distribution<int> random_h { y_min, y_max };
    std::random_device dev;
    std::mt19937 engine { dev() };

    auto x = random_w(engine);
    if ( x < 0 ) {
        x += grid_width;
    };
    x = fmod(x, grid_width);

    auto y = random_h(engine);
    if ( y < 0 ) {
        y += grid_height;
    }
    y = fmod(y, grid_height);

    return SDL_Point { x, y };
}
