#include <iostream>
#include <regex>
#include <string>
#include "httplib.h"
#include "../interpreter/token.hpp"
#include "../interact/demodulator.hpp"
#include "../interact/modulator.hpp"

// TODO:

const int GAME_FINISHED = 2;

struct GameResponse{
	int status; // wrong なら 0
	int gameStage; // 0-> not started, 1-> started, 2->finished
	std::vector<int> staticGameInfo; // (x0, role, x2, x3, x4)
	// role 0-> attacker, 1->defender
	//std::vector<int> gameState; // (gameTick, x1, shipsAndCommands) 
};

// 10進数playerKey -> [2, playerKey, nil]
extern std::string makeJoinRequest(std::string playerKey);
// [3, playerKey, [x0, x1, x2, x3]]
extern std::string makeStartRequest(std::string playerKey, GameResponse gameResponse);
// [4, playerKey, [commands]]
extern std::string makeCommandsRequest(std::string playerKey, GameResponse gameResponse);
// token 列 -> GameResponse
extern GameResponse getGameResponse(std::vector<Token> tokens);

template <typename T>
void printv(const std::vector<T> &v) {
  int sz = v.size();
  for (int i = 0; i < sz; i++) {
    cout << v[i] << " \n"[i == sz - 1];
  }
}

void dump_game(GameResponse &gameResponse){
	std::cout << "status: " << gameResponse.status << std::endl;
	std::cout << "gameStage: " << gameResponse.gameStage << std::endl;
	std::cout << "staticGameInfo: "; printv(gameResponse.staticGameInfo);
}

int main(int argc, char* argv[])
{
	const std::string serverUrl(argv[1]);
	const std::string playerKey(argv[2]);

	std::cout << "ServerUrl: " << serverUrl << "; PlayerKey: " << playerKey << std::endl;
	
	const std::regex urlRegexp("http://(.+):(\\d+)");
	std::smatch urlMatches;
	if (!std::regex_search(serverUrl, urlMatches, urlRegexp) || urlMatches.size() != 3) {
		std::cout << "Unexpected server response:\nBad server URL" << std::endl;
		return 1;
	}
	const std::string serverName = urlMatches[1];
	const int serverPort = std::stoi(urlMatches[2]);
	httplib::Client client(serverName, serverPort);

    // make valid JOIN request using the provided playerKey
	auto joinRequest = makeJoinRequest(playerKey);
	// [2, playerKey, nil] -> 11...
	joinRequest = modulate(joinRequest);
    // send it to aliens and get the GameResponse
	auto serverResponse = client.Post(serverUrl.c_str(), joinRequest.c_str(), "text/plain");
	if (!serverResponse) {
		std::cout << "Unexpected server gameResponse:\nNo response from server (join)" << std::endl;
		return 1;
	}
	if (serverResponse->status != 200) {
		std::cout << "Unexpected server response:\nHTTP code (join): " << serverResponse->status
		          << "\nResponse body: " << serverResponse->body << std::endl;
		return 2;
	}
	std::cout << "joinRequest Response: " << serverResponse->body << std::endl;
	auto gameResponse = getGameResponse(demodulate(serverResponse->body));
	dump_game(gameResponse);

    // make valid START request using the provided playerKey and gameResponse returned from JOIN
	auto startRequest = makeStartRequest(playerKey, gameResponse);
	// [3, playerKey, [x0, x1, x2, x3]] -> 11...
	startRequest = modulate(startRequest);
	// send it to aliens and get the updated GameResponse
	serverResponse = client.Post(serverUrl.c_str(), startRequest.c_str(), "text/plain");
	if (!serverResponse) {
		std::cout << "Unexpected server gameResponse:\nNo response from server (start)" << std::endl;
		return 1;
	}
	if (serverResponse->status != 200) {
		std::cout << "Unexpected server response:\nHTTP code (start): " << serverResponse->status
		          << "\nResponse body: " << serverResponse->body << std::endl;
		return 2;
	}
	std::cout << "startRequest Response: " << serverResponse->body << std::endl;
	gameResponse = getGameResponse(demodulate(serverResponse->body));
	dump_game(gameResponse);

	while(true){
        // make valid COMMANDS request using the provided playerKey and gameResponse returned from START or previous COMMANDS
        auto commandsRequest = makeCommandsRequest(playerKey, gameResponse);
		// [4, playerKey, nil] -> ..
		commandsRequest = modulate(commandsRequest);

        // send it to aliens and get the updated GameResponse
        serverResponse = client.Post(serverUrl.c_str(), commandsRequest.c_str(), "text/plain");
		if (!serverResponse) {
			std::cout << "Unexpected server gameResponse:\nNo response from server (command)" << std::endl;
			return 1;
		}
		if (serverResponse->status != 200) {
			std::cout << "Unexpected server response:\nHTTP code (start): " << serverResponse->status
					<< "\nResponse body: " << serverResponse->body << std::endl;
			return 2;
		}
		std::cout << "commandsRequestResponse: " << serverResponse->body << std::endl;
		gameResponse = getGameResponse(demodulate(serverResponse->body));
		dump_game(gameResponse);
		if(gameResponse.gameStage == GAME_FINISHED){
			std::cout << "game finished" << std::endl;
			break;
		}
	}
	return 0;
}
