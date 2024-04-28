#include "player.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <regex>

std::vector<Player> ParsePlayersFromFile()
{
    std::ifstream input { "highest_scores.txt" };
    std::string line;

    std::vector<Player> players;

    while( std::getline(input, line) ) {
        std::stringstream ss { line };
        std::string username;
        ss >> username;

        std::map<Level, size_t> highest_scores;
        std::regex lvl_score_regex { "[a-z]+:[0-9]+" };

        for ( std::string lvl_score = ""; ss >> lvl_score; lvl_score ) {
            if ( !std::regex_match(lvl_score, lvl_score_regex) ) {
                continue;
            }

            std::replace(std::begin(lvl_score), std::end(lvl_score), ':', ' ');

            std::istringstream lvl_score_ss { lvl_score };
            std::string lvl;
            size_t score;
            lvl_score_ss >> lvl >> score;
            highest_scores[FromFileString(lvl)] = score;
        }

        players.emplace_back(std::move(username), std::move(highest_scores));
    }

    return players;
}

void PersistPlayersToFile(const std::vector<Player>& players)
{
    std::ofstream input { "highest_scores.txt" };
    const auto& levels = AllLevels();

    for(const auto& p : players) {
        // format in the following way:
        // beginner:<score_value> medium:<score_value> advanced:<score_value>
        std::stringstream ss;
        for(const auto& lvl : levels) {
            ss << ToString(lvl) << ":" << p.HighestScore(lvl) << " ";
        }
        input << p.Username() << " " << ss.str() << std::endl;
    }
}

Player Player::CreatePlayer()
{
    std::string input;
    std::regex regex { "[a-zA-Z0-9]{5,10}" };

    std::cout << "Please provide username for player: " << std::endl;

    while ( std::getline(std::cin, input) ) {
        if ( std::regex_match(input, regex) ) {
            break;
        }

        std::cerr << "Please provide from 5 up to 10 characters (English letters and digits)!!!" << std::endl;
    }

    auto players = ParsePlayersFromFile();
    for(auto& p : players) {
        if ( p.Username() == input ) {
            // player has already played before, so 'reuse old' instance
            return std::move(p);
        }
    }

    // playing for the 1st time - create new instance
    return Player { std::move(input) };
}

Player::Player(std::string&& username)
    : username { std::move(username) } {}

Player::Player(std::string&& username, std::map<Level, size_t>&& highest_scores)
    : username { std::move(username) },
      highest_scores { std::move(highest_scores) } {}

const std::string& Player::Username() const
{
    return username;
}

size_t Player::HighestScore(Level lvl) const
{
    if ( highest_scores.find(lvl) == std::end(highest_scores) ) {
        return 0u;
    }

    return highest_scores.at(lvl);
}

void Player::UpdateHighestScore(Level lvl, size_t new_score)
{
    if ( highest_scores.find(lvl) == std::end(highest_scores) ) {
        highest_scores[lvl] = new_score;
    }
    else if ( new_score > highest_scores[lvl] ) {
        highest_scores[lvl] = new_score;
    }
}

void Player::PersistHighestScore()
{
    bool score_updated = false;

    auto players = ParsePlayersFromFile();
    const auto& levels = AllLevels();

    for(auto& p : players) {
        if ( p.Username() == username ) {
            for(const auto& lvl : levels) {
                // maybe the highest score in file is now obsolete
                // and thus we try to update it here...
                const auto curr_score = HighestScore(lvl);
                p.UpdateHighestScore(lvl, curr_score);
                score_updated = true;
            }
        }
    }

    if ( !score_updated ) {
        // this player did not play before
        // so we have to persist for him as well
        players.emplace_back(*this);
    }

    PersistPlayersToFile(players);
}

// sort first according to the highest score for beginner then medium then advanced level
bool operator<(const Player& left, const Player& right)
{
    const auto& levels = AllLevels();
    for(const auto& lvl : levels) {
        if ( left.HighestScore(lvl) != right.HighestScore(lvl) ) {
            return left.HighestScore(lvl) < right.HighestScore(lvl);
        }
    }

    return false;
}
