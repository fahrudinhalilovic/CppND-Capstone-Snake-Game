#ifndef BEGINNER_OBSTACLES_GENERATOR
#define BEGINNER_OBSTACLES_GENERATOR

#include <random>

#include "../obstacles_generator.h"

class AdvancedObstaclesGenerator : public ObstaclesGenerator
{
public:
    AdvancedObstaclesGenerator(size_t grid_w, size_t grid_h);

    void CreateObstacle(size_t snake_x, size_t snake_y) override;

private:
    size_t grid_width;
    size_t grid_height;

    std::random_device dev;
    std::mt19937 engine;
};

#endif
