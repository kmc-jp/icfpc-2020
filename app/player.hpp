#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <vector>
#include "command.hpp"
#include "point2d.hpp"
#include "response.hpp"

std::vector<CommandPtr> Player(GameResponse gameResponse);

#endif