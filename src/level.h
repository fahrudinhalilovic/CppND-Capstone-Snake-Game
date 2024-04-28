#ifndef LEVEL
#define LEVEL

#include <string>
#include <vector>

enum class Level {
    Beginner,
    Medium,
    Advanced
};

std::string ToString(Level lvl);
Level FromFileString(const std::string& lvl);
Level FromInputString(const std::string& lvl);
const std::vector<Level>& AllLevels();

#endif
