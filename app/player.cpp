#include "player.hpp"

std::vector<Command> Player(GameResponse gameResponse) {

  if (gameResponse.status == 0) return {};
  if (gameResponse.gameStage != 1) return {};

  std::vector<Command> com;

  int myRole = gameResponse.staticGameInfo[1];
  for(auto &sc : gameResponse.gameState.shipsAndCommands){
    Ship &ship = sc.ship;
    if (ship.role == myRole) {
      com.push_back(Accelerate(ship.shipId, {ship.velocity.x, ship.velocity.y}));
    }
  }

  return com;
}
