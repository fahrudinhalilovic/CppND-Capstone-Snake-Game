#include "advanced.h"

#include "../snake.h"

AdvancedObstaclesGenerator::AdvancedObstaclesGenerator(size_t grid_w, size_t grid_h)
    : grid_width { grid_w },
      grid_height { grid_h },
      engine { dev() } { }

void AdvancedObstaclesGenerator::CreateObstacle(size_t snake_x, size_t snake_y)
{
    constexpr size_t offset = 3;

    int x_min = snake_x;
    int x_max = snake_x;
    int y_min = snake_y;
    int y_max = snake_y;

    // just use it like this for now...
    auto snake_direction = Snake::Direction::kUp;

    switch ( snake_direction ) {
        case Snake::Direction::kUp:
            x_max = snake_x - offset - 1;
            x_min = snake_x - offset - 3;
            y_min = snake_y - 1;
            y_max = snake_y + 1;
            break;
        case Snake::Direction::kDown:
            x_max = snake_x + offset + 1;
            x_min = snake_x + offset + 3;
            y_min = snake_y - 1;
            y_max = snake_y + 1;
            break;
        case Snake::Direction::kLeft:
            x_max = snake_x - 1;
            x_min = snake_x + 1;
            y_min = snake_y - offset - 1;
            y_max = snake_y - offset - 3;
            break;
        case Snake::Direction::kRight:
            x_max = snake_x - 1;
            x_min = snake_x + 1;
            y_min = snake_y + offset + 1;
            y_max = snake_y + offset + 3;
            break;

    }

    std::uniform_int_distribution<int> random_w { x_min, x_max };
    std::uniform_int_distribution<int> random_h { y_min, y_max };

    auto x = random_w(engine);
    auto y = random_h(engine);
    

    // TODO: check if food occupied this place
    // TODO: check if snake occupied this place
    // TODO: check if another obstacle occupied this place

    // TODO: if place is free -> occupy it with new obstacle
}
