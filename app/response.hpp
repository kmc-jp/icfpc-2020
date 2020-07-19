#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <vector>
#include "../interact/demodulator.hpp"
#include "command.hpp"
#include "point2d.hpp"

struct Ship {
  int role;  // 0-> attacker, 1->defender
  int64_t shipId;
  point position;
  point velocity;
  int x4, x5, x6, x7;
};

struct ShipAndCommand {
  Ship ship;  // ship state description
  std::vector<CommandPtr> appliedCommands;  // list of commands applied to the
                                            // ship on the previous tick
};

struct GameState {
  int64_t gameTick;  // time inside the game
  int64_t x1;        // unknown
  std::vector<ShipAndCommand> shipsAndCommands;
};

struct GameResponse {
  int status;                       // wrong なら 0
  int gameStage;                    // 0-> not started, 1-> started, 2->finished
  std::vector<int> staticGameInfo;  // (x0, role, x2, x3, x4)
  GameState gameState;              // (gameTick, x1, shipsAndCommands)
};

// token 列 -> GameResponse
GameResponse getGameResponse(AlienData data);

#endif
