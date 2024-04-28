#ifndef BEGINNER_OBSTACLES_GENERATOR
#define BEGINNER_OBSTACLES_GENERATOR

#include "../obstacles_generator.h"

class BeginnerObstaclesGenerator : public ObstaclesGenerator
{
public:
    BeginnerObstaclesGenerator() = default;

    std::optional<SDL_Point> CreateObstacle() const override;
};

#endif
