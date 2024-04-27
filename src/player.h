#ifndef PLAYER
#define PLAYER

#include <string>
#include <vector>
#include <map>

enum class Level {
    Beginner,
    Medium,
    Advanced
};

std::string ToString(Level lvl);
Level FromFileString(const std::string& lvl);
Level FromInputString(const std::string& lvl);
const std::vector<Level>& AllLevels();

class Player {

public:
    explicit Player(std::string&& username);
    Player(std::string&& username, std::map<Level, size_t>&& highest_scores);

    const std::string& Username() const;
    size_t HighestScore(Level lvl) const;
    void UpdateHighestScore(Level lvl, size_t new_score);
    void PersistHighestScore();

    static Player CreatePlayer();

private:
    std::string username;
    std::map<Level, size_t> highest_scores;
};


bool operator<(const Player& left, const Player& right);

#endif