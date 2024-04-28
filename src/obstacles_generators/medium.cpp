#include "medium.h"

MediumObstaclesGenerator::MediumObstaclesGenerator(size_t grid_width, size_t grid_height)
    : engine { dev() },
      random_w { 0, static_cast<int>(grid_width - 1) },
      random_h { 0, static_cast<int>(grid_height - 1) } { }

void MediumObstaclesGenerator::CreateObstacle(size_t snake_x, size_t snake_y)
{
    auto x = random_w(engine);
    auto y = random_h(engine);

    // TODO: check if food occupied this place
    // TODO: check if snake occupied this place
    // TODO: check if another obstacle occupied this place

    // TODO: if place is free -> occupy it with new obstacle
}
