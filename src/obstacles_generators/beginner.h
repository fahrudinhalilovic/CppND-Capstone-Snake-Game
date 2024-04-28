#ifndef BEGINNER_OBSTACLES_GENERATOR
#define BEGINNER_OBSTACLES_GENERATOR

#include "../obstacles_generator.h"

class BeginnerObstaclesGenerator : public ObstaclesGenerator
{
public:
    BeginnerObstaclesGenerator() = default;

    void CreateObstacle(size_t snake_x, size_t snake_y) override;
};

#endif
