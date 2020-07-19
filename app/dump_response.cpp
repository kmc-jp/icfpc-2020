#include "dump_response.hpp"
#include <cassert>
#include <iostream>

template <typename T>
void printv(const std::vector<T> &v) {
  int sz = v.size();
  for (int i = 0; i < sz; i++) {
    std::cout << v[i] << " \n"[i == sz - 1];
  }
}

void dump_point(point p) { std::cout << p.to_string() << std::endl; }

void dump_ship(Ship ship) {
  // std::cout << "dump ship" << std::endl;
  std::cout << "role: " << ship.role << std::endl;
  std::cout << "shipId: " << ship.shipId << std::endl;
  std::cout << "position: ";
  dump_point(ship.position);
  std::cout << "velocity: ";
  dump_point(ship.velocity);
  std::cout << "x4, x5, x6, x7: " << ship.x4 << " " << ship.x5 << " " << ship.x6
            << " " << ship.x7 << std::endl;
}

void dump_ship_and_command(ShipAndCommand sac) {
  // std::cout << "dump ship and command" << std::endl;
  dump_ship(sac.ship);
}

void dump_game_state(GameState gameState) {
  // std::cout << "dump gameStates" << std::endl;
  std::cout << "gameTick: " << gameState.gameTick << std::endl;
  std::cout << "x1: " << gameState.x1 << std::endl;
  for (auto sac : gameState.shipsAndCommands) {
    dump_ship_and_command(sac);
  }
}

void dump_game_response(GameResponse gameResponse) {
  std::cout << "status: " << gameResponse.status << std::endl;
  std::cout << "gameStage: " << gameResponse.gameStage << std::endl;
  std::cout << "staticGameInfo: ";
  printv(gameResponse.staticGameInfo);
  std::cout << "game response: " << std::endl;
  dump_game_state(gameResponse.gameState);
}

/*
int main() {
  std::string s =
      "110110000111011000011111011110000100000000110101111011110001000000000110"
      "110000111011100100000000111101110000100001101110100000000011110110000111"
      "011000011101100001110110000100001111010111101110000100001101110100000000"
      "011111111011000011101011110110001101110001100001111010010111101100001110"
      "110000111011000011101100001001101011011100100000011011000010011000011111"
      "101011011000011111101000111011000110000111101001011110110000111011000011"
      "10110000111011000010011010110111001000000110110000100110000000000";
  auto gameResponse = getGameResponse(demodulateList(s));
  dump_game_response(gameResponse);
}
->
status: 1
gameStage: 1
staticGameInfo: 256 0 64 128 1
game response:
gameTick: 0
x1: 128
role: 1
shipId: 0
position: (3, 48)
velocity: (0, 0)
role: 0
shipId: 1
position: (-3, -48)
velocity: (0, 0)
*/
