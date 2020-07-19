#include "player.hpp"

std::vector<CommandPtr> Player(GameResponse gameResponse) {

  if (gameResponse.status == 0) return {};
  if (gameResponse.gameStage != 1) return {};

  std::vector<CommandPtr> com;

  int myRole = gameResponse.staticGameInfo[1];
  for(auto &sc : gameResponse.gameState.shipsAndCommands){
    Ship &ship = sc.ship;
    if (ship.role == myRole) {
      com.push_back(make_accelerate(ship.shipId, {ship.velocity.x, ship.velocity.y}));
    }
  }

  return com;
}
