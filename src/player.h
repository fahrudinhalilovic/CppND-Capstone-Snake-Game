#ifndef PLAYER
#define PLAYER

#include <string>

class Player {

public:
    explicit Player(std::string&& username);
    Player(std::string&& username, size_t highest_score);

    const std::string& Username() const;
    size_t HighestScore() const;
    void UpdateHighestScore(size_t new_score);
    void PersistHighestScore();

    static Player CreatePlayer();

private:
    std::string username;
    size_t highest_score = 0;
};


bool operator<(const Player& left, const Player& right);

#endif