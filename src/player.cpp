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
        std::string u;
        size_t highest_score;
        ss >> u >> highest_score;

        players.emplace_back(std::move(u), highest_score);
    }

    return players;
}

void PersistPlayersToFile(const std::vector<Player>& players)
{
    std::ofstream input { "highest_scores.txt" };

    for(const auto& p : players) {
        input << p.Username() << " " << p.HighestScore() << std::endl;
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

Player::Player(std::string&& username, size_t highest_score)
    : username { std::move(username) },
      highest_score { highest_score } {}

const std::string& Player::Username() const
{
    return username;
}

size_t Player::HighestScore() const
{
    return highest_score;
}

void Player::UpdateHighestScore(size_t new_score)
{
    if ( new_score > highest_score ) {
        highest_score = new_score;
    }
}

void Player::PersistHighestScore()
{
    bool score_updated = false;

    auto players = ParsePlayersFromFile();
    for(auto& p : players) {
        if ( p.Username() == username ) {
            // maybe the highest score in file is now obsolete
            // and thus we try to update it here...
            p.UpdateHighestScore(highest_score);
            score_updated = true;
        }
    }

    if ( !score_updated ) {
        // this player did not play before
        // so we have to persist for him as well
        players.emplace_back(*this);
    }

    PersistPlayersToFile(players);
}

bool operator<(const Player& left, const Player& right)
{
    return left.HighestScore() < right.HighestScore();
}
