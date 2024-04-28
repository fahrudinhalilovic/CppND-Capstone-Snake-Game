#ifndef BEGINNER_OBSTACLES_GENERATOR
#define BEGINNER_OBSTACLES_GENERATOR

#include <random>

#include "../obstacles_generator.h"

class MediumObstaclesGenerator : public ObstaclesGenerator
{
public:
    MediumObstaclesGenerator(size_t grid_width, size_t grid_height);

    void CreateObstacle(size_t snake_x, size_t snake_y) override;

private:
    std::random_device dev;
    std::mt19937 engine;
    std::uniform_int_distribution<int> random_w;
    std::uniform_int_distribution<int> random_h;
};

#endif
