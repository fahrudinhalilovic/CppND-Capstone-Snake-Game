#ifndef ADVANCED_OBSTACLES_GENERATOR
#define ADVANCED_OBSTACLES_GENERATOR

#include <random>

#include "../obstacles_generator.h"

class AdvancedObstaclesGenerator : public ObstaclesGenerator
{
public:
    AdvancedObstaclesGenerator(size_t grid_w, size_t grid_h, const Snake& s);

    std::optional<SDL_Point> CreateObstacle() const override;

private:
    size_t grid_width;
    size_t grid_height;

    const Snake& snake;
};

#endif
