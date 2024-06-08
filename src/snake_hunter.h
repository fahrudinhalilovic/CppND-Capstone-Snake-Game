
#ifndef SNAKE_HUNTER
#define SNAKE_HUNTER

#include "snake.h"

enum class FieldType {
    Empty,
    Snake,
    Obstacle,
    Food,
    SnakeHunter
};

using ReconstructionSite = std::vector<std::vector<std::pair<int,int>>>;

class SnakeHunter {
public:
    SnakeHunter(size_t grid_width, size_t grid_height);

    void Hunt(const Snake& snake, const std::vector<SDL_Point>& obstacles, const SDL_Point& food);

    SDL_Point CurrentPosition() const;

    bool SnakeHunterCell(int x, int y) const;

private:

    void ReconstructPathFromSnakeToHunter(const Snake& snake,
                                          const SDL_Point& intersection_pt,
                                          const std::vector<std::vector<FieldType>>& field,
                                          const ReconstructionSite& reconstructionSite);

private:
    size_t grid_width;
    size_t grid_height;

    // location of the hunter
    double x;
    double y;
};

#endif