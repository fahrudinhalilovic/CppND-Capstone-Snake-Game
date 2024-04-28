#ifndef OBSTACLES_GENERATOR
#define OBSTACLES_GENERATOR

#include <memory>

#include "level.h"

class ObstaclesGenerator
{
public:
    using SPtr = std::shared_ptr<ObstaclesGenerator>;

    virtual ~ObstaclesGenerator() = default;

    virtual void CreateObstacle(size_t snake_x, size_t snake_y) = 0;

    static SPtr CreateObstaclesGenerator(Level lvl);
};


#endif
