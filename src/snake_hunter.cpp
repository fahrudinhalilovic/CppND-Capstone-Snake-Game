#include "snake_hunter.h"

#include <queue>
#include <cassert>
#include <iostream>

SnakeHunter::SnakeHunter(size_t g_width, size_t g_height, const Snake& s)
    : grid_width { g_width },
      grid_height { g_height },
      snake { s },
      x { 0 },
      y { 0 }  {}

void normalizePoint(size_t grid_height, size_t grid_width, int& x, int& y)
{
    x += grid_height;
    y += grid_width;
    x = std::fmod(x, grid_height);
    y = std::fmod(y, grid_width);
}

void SnakeHunter::Hunt(const std::vector<SDL_Point>& obstacles, const SDL_Point& food)
{
    if ( !snake.alive ) {
        return;
    }

    std::vector<std::vector<FieldType>> field { grid_height, std::vector<FieldType> { grid_width, FieldType::Empty } };
    ReconstructionSite reconstruction_site { grid_height, std::vector<std::pair<int,int>> { grid_width, {0, 0} } };

    field[food.y][food.x] = FieldType::Food;

    for( const auto& o : obstacles ) {
        field[o.y][o.x] = FieldType::Obstacle;
    }

    field[snake.head_y][snake.head_x] = FieldType::Snake;
    for( const auto& s : snake.body ) {
        field[s.y][s.x] = FieldType::Snake;
    }

    field[y][x] = FieldType::SnakeHunter;

    std::queue<SDL_Point> queue;
    queue.push(SDL_Point { static_cast<int>(y), static_cast<int>(x) });

    auto snake_found = false;

    const static std::vector<int> x_movement { -1, 1, 0, 0 };
    const static std::vector<int> y_movement { 0, 0, -1, 1 };

    while ( !queue.empty() ) {
        const auto curr_pt = queue.front();
        queue.pop();

        for ( auto idx = 0u; idx < x_movement.size(); ++idx ) {
            SDL_Point new_pt { curr_pt.x + x_movement[idx], curr_pt.y + y_movement[idx] };
            normalizePoint(grid_height, grid_width, new_pt.x, new_pt.y);

            const auto curr_field = field[new_pt.x][new_pt.y];
            if ( curr_field == FieldType::Snake ) {
                reconstruction_site[new_pt.x][new_pt.y] = std::make_pair(x_movement[idx], y_movement[idx]);
                // we found the closest path to the snake
                snake_found = true;
                reconstructPathFromSnakeToHunter(new_pt, field, reconstruction_site);
                break;
            }
            else if ( curr_field == FieldType::Food || curr_field == FieldType::Obstacle || curr_field == FieldType::SnakeHunter ) {
                // hunter can not use this field
                continue;
            }
            else {
                assert ( curr_field == FieldType::Empty );
                reconstruction_site[new_pt.x][new_pt.y] = {x_movement[idx], y_movement[idx]};
                field[new_pt.x][new_pt.y] = FieldType::SnakeHunter;
                queue.push(std::move(new_pt));
            }
        }

        if ( snake_found ) {
            break;
        }
    }
}

void SnakeHunter::reconstructPathFromSnakeToHunter(const SDL_Point& intersection_pt,
                                                   const std::vector<std::vector<FieldType>>& field,
                                                   const ReconstructionSite& reconstruction_site)
{
    auto curr_pt = intersection_pt;
    auto last_movement = reconstruction_site[curr_pt.x][curr_pt.y];

    while ( curr_pt.x != static_cast<int>(y) || curr_pt.y != static_cast<int>(x) ) {
        const auto& movement = reconstruction_site[curr_pt.x][curr_pt.y];
        last_movement = movement;

        auto new_x = curr_pt.x - movement.first;
        auto new_y = curr_pt.y - movement.second;
        normalizePoint(grid_height, grid_width, new_x, new_y);
        curr_pt = SDL_Point { new_x, new_y };
    }

    auto new_x = x + ( snake.speed / 2 ) * last_movement.second;
    if ( new_x >= grid_width ) {
        new_x -= grid_width;
    }
    if ( new_x < 0 ) {
        new_x += grid_width;
    }

    auto new_y = y + ( snake.speed / 2 ) * last_movement.first;
    if ( new_y >= grid_height ) {
        new_y -= grid_height;
    }
    if ( new_y < 0 ) {
        new_y += grid_height;
    }

    x = new_x;
    y = new_y;
}

SDL_Point SnakeHunter::CurrentPosition() const
{
    return SDL_Point { static_cast<int>(x), static_cast<int>(y) };
}

bool SnakeHunter::SnakeHunterCell(int input_x, int input_y) const
{
    return static_cast<int>(x) == input_x && static_cast<int>(y) == input_y;
}