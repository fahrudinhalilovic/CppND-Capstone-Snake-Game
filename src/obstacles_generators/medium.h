#ifndef MEDIUM_OBSTACLES_GENERATOR
#define MEDIUM_OBSTACLES_GENERATOR

#include <random>

#include "../obstacles_generator.h"

class MediumObstaclesGenerator : public ObstaclesGenerator
{
public:
    MediumObstaclesGenerator(size_t grid_w, size_t grid_h);

    std::optional<SDL_Point> CreateObstacle() const override;

private:
    size_t grid_width;
    size_t grid_height;
};

#endif
