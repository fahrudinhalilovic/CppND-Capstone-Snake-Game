#include "obstacles_generator.h"

#include "obstacles_generators/beginner.h"
#include "obstacles_generators/medium.h"
#include "obstacles_generators/advanced.h"

ObstaclesGenerator::SPtr ObstaclesGenerator::ObstaclesFactory(Level lvl, size_t grid_width, size_t grid_height, const Snake& s)
{
    switch ( lvl ) {
        case Level::Beginner:
            return std::make_shared<BeginnerObstaclesGenerator>();
        case Level::Medium:
            return std::make_shared<MediumObstaclesGenerator>(grid_width, grid_height);
        case Level::Advanced:
            return std::make_shared<AdvancedObstaclesGenerator>(grid_width, grid_height, s);
    }

    return nullptr;
}
