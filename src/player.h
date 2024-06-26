#ifndef PLAYER
#define PLAYER

#include <string>
#include <vector>
#include <map>

#include "level.h"

class Player {

public:
    explicit Player(std::string username);
    Player(std::string username, std::map<Level, size_t> highest_scores);

    const std::string& Username() const;
    size_t HighestScore(Level lvl) const;
    void UpdateHighestScore(Level lvl, size_t new_score);
    void PersistHighestScoresToFile(Level lvl);

    static Player CreatePlayer();

private:
    std::string username;
    std::map<Level, size_t> highest_scores;
};

#endif