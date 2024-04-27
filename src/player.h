#ifndef PLAYER
#define PLAYER

#include <string>

class Player {

public:
    Player(std::string&& username);

    size_t HighestScore() const;

private:
    static size_t ParseHighestScoresFromFile(const std::string& username);

private:
    std::string username;
    size_t highest_score;
};


bool operator<(const Player& left, const Player& right);

#endif