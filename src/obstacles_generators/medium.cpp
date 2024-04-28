#include "medium.h"

MediumObstaclesGenerator::MediumObstaclesGenerator(size_t grid_w, size_t grid_h)
    : grid_width { grid_w },
      grid_height { grid_h } {}

std::optional<SDL_Point> MediumObstaclesGenerator::CreateObstacle() const
{
    std::uniform_int_distribution<int> random_w { 0, static_cast<int>(grid_width - 1) };
    std::uniform_int_distribution<int> random_h { 0, static_cast<int>(grid_height - 1) };
    std::random_device dev;
    std::mt19937 engine { dev() };

    auto x = random_w(engine);
    auto y = random_h(engine);
    return SDL_Point { x, y };
}
