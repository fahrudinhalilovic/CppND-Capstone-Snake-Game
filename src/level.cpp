#include "level.h"

#include <stdexcept>

std::string ToString(Level lvl)
{
    switch ( lvl ) {
        case Level::Beginner:
            return "beginner";
        case Level::Medium:
            return "medium";
        case Level::Advanced:
            return "advanced";
    }

    // should not end up here at all...
    return "";
}

Level FromFileString(const std::string& lvl)
{
    if ( lvl == ToString(Level::Beginner) ) {
        return Level::Beginner;
    }
    else if ( lvl == ToString(Level::Medium) ) {
        return Level::Medium;
    }
    else if ( lvl == ToString(Level::Advanced) ) {
        return Level::Advanced;
    }

    // invalid input really...
    throw std::invalid_argument { "Invalid input string: " + lvl };

    return Level::Medium;
}

Level FromInputString(const std::string& lvl)
{
    if ( lvl == "1" ) {
        return Level::Beginner;
    }
    else if ( lvl == "2" ) {
        return Level::Medium;
    }
    else if ( lvl == "3" ) {
        return Level::Advanced;
    }

    // invalid input really...
    throw std::invalid_argument { "Invalid input string: " + lvl };

    return Level::Medium;
}

const std::vector<Level>& AllLevels()
{
    static const std::vector<Level> levels { Level::Beginner, Level::Medium, Level::Advanced };
    return levels;
}
