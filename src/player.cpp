#include "player.h"

#include <fstream>
#include <sstream>

Player::Player(std::string&& username)
    : username { std::move(username) } {}

size_t Player::HighestScore() const
{
    return highest_score;
}

size_t Player::ParseHighestScoresFromFile(const std::string& username)
{
    std::ifstream input { "highest_scores.txt" };
    std::string line;

    while( std::getline(input, line) ) {
        std::stringstream ss { line };
        std::string u;
        ss >> u;

        if ( username == u ) {
            size_t highest_score = 0u;
            ss >> highest_score;
            return highest_score;
        }
    }

    return 0u;
}

bool operator<(const Player& left, const Player& right)
{
    return left.HighestScore() < right.HighestScore();
}
