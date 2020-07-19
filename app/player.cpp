#include "player.hpp"

int sign(int x){
  if (x > 0) return 1;
  else if (x < 0) return -1;
  else return 0;
}

std::vector<CommandPtr> Player(GameResponse gameResponse) {

  if (gameResponse.status == 0) return {};
  if (gameResponse.gameStage != 1) return {};

  std::vector<CommandPtr> com;

  int myRole = gameResponse.staticGameInfo[1];
  for(auto &sc : gameResponse.gameState.shipsAndCommands){
    Ship &ship = sc.ship;
    if (ship.role == myRole) {
      com.push_back(make_accelerate(ship.shipId, {sign(ship.position.x), sign(ship.position.y)}));
    }
  }

  return com;
}
