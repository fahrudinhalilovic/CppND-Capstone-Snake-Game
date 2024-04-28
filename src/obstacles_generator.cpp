#include "obstacles_generator.h"

ObstaclesGenerator::SPtr ObstaclesGenerator::CreateObstaclesGenerator(Level lvl)
{
    switch ( lvl ) {
        case Level::Beginner:
            return nullptr;
        case Level::Medium:
            return nullptr;
        case Level::Advanced:
            return nullptr;
    }

    return nullptr;
}
