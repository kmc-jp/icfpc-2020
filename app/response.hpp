#include "../interact/demodulator.hpp"
#include <vector>

struct GameResponse{
	int status; // wrong なら 0
	int gameStage; // 0-> not started, 1-> started, 2->finished
	std::vector<int> staticGameInfo; // (x0, role, x2, x3, x4)
	// role 0-> attacker, 1->defender
	//std::vector<int> gameState; // (gameTick, x1, shipsAndCommands) 
};

// token 列 -> GameResponse
GameResponse getGameResponse(AlienData data);
