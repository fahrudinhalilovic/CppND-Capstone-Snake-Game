#ifndef OBSTACLES_GENERATOR
#define OBSTACLES_GENERATOR

#include <memory>
#include <optional>

#include "level.h"
#include "SDL.h"

class Snake;

class ObstaclesGenerator
{
public:
    using SPtr = std::shared_ptr<ObstaclesGenerator>;

    virtual ~ObstaclesGenerator() = default;

    virtual std::optional<SDL_Point> CreateObstacle() const = 0;

    static SPtr CreateObstaclesGenerator(Level lvl, size_t grid_width, size_t grid_height, const Snake& s);
};

#endif
