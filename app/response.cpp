#include "response.hpp"
#include <cassert>

point getPoint(AlienData data) {
  point res;
  res.x = data.vec[0].num;
  res.y = data.vec[1].num;
  return res;
}

Ship getShip(AlienData data) {
  assert(data.type == AlienDataType::List);
  assert(data.vec.size() == 8);
  Ship res;
  res.role = data.vec[0].num;
  res.shipId = data.vec[1].num;
  res.position = getPoint(data.vec[2]);
  res.velocity = getPoint(data.vec[3]);
  res.x4 = data.vec[4].num;
  res.x5 = data.vec[5].num;
  res.x6 = data.vec[6].num;
  res.x7 = data.vec[7].num;
  return res;
}

ShipAndCommand getShipAndCommand(AlienData data) {
  assert(data.type == AlienDataType::List);
  assert(data.vec.size() == 2);
  ShipAndCommand res;
  res.ship = getShip(data.vec[0]);
  return res;
}

GameState getGameState(AlienData data) {
  assert(data.type == AlienDataType::List);
  assert(data.vec.size() == 3);
  GameState res;
  res.gameTick = data.vec[0].num;
  res.x1 = data.vec[1].num;
  int sz = data.vec[2].vec.size();
  assert(data.vec[2].type == AlienDataType::List);
  for (int i = 0; i < sz; i++) {
    res.shipsAndCommands.push_back(getShipAndCommand(data.vec[2].vec[i]));
  }
  return res;
}

GameResponse getGameResponse(AlienData data) {
  GameResponse res;
  assert(data.type == AlienDataType::List);
  assert(data.vec.size() >= 1);
  res.status = data.vec[0].num;
  if (res.status == 0) return res;
  assert(data.vec.size() == 4);
  res.gameStage = data.vec[1].num;
  assert(data.vec[2].type == AlienDataType::List);
  assert(data.vec[2].vec.size() == 5);
	if (res.gameStage == 2) return res;
  for (int i = 0; i < 5; i++) {
    res.staticGameInfo.push_back(data.vec[2].vec[i].num);
  }
	if (res.gameStage == 0) return res;
  res.gameState = getGameState(data.vec[3]);
  return res;
}
