#include <iostream>
#include <regex>
#include <string>
#include "httplib.h"
#include "../interpreter/token.hpp"
#include "../interact/demodulator.hpp"
#include "../interact/modulator.hpp"
#include "response.hpp"

const int GAME_FINISHED = 2;

// 10進数playerKey -> [2, playerKey, nil]
std::string makeJoinRequest(std::string playerKey){
	return "[2," + playerKey + ",Nil,Nil]";
}

// [3, playerKey, [x0, x1, x2, x3]]
// We noticed, that START doesn’t finish successfully when x3 is 0 or xi’s are too large.
std::string makeStartRequest(std::string playerKey, GameResponse gameResponse){
	return "[3," + playerKey + ",[1,1,1,1,Nil],Nil]";
}

// [4, playerKey, [commands]]
std::string makeCommandsRequest(std::string playerKey, GameResponse gameResponse){
	return "[4," + playerKey + ",Nil,Nil]";
}

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
	//std::cout << makeStartRequest("114514", GameResponse()) << std::endl;
	//std::cout << makeJoinRequest("114514") << std::endl;
	//std::cout << makeCommandsRequest("114514", GameResponse()) << std::endl;
	
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
	std::cout << "joinRequest: " << joinRequest << std::endl;
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
	auto gameResponse = getGameResponse(demodulateList(serverResponse->body));
	dump_game(gameResponse);

    // make valid START request using the provided playerKey and gameResponse returned from JOIN
	auto startRequest = makeStartRequest(playerKey, gameResponse);
	std::cout << "startRequest: " << startRequest << std::endl;
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
	gameResponse = getGameResponse(demodulateList(serverResponse->body));
	dump_game(gameResponse);

	while(true){
        // make valid COMMANDS request using the provided playerKey and gameResponse returned from START or previous COMMANDS
        auto commandsRequest = makeCommandsRequest(playerKey, gameResponse);
		std::cout << "commandRequest: " << commandsRequest << std::endl;
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
		gameResponse = getGameResponse(demodulateList(serverResponse->body));
		dump_game(gameResponse);
		if(gameResponse.gameStage == GAME_FINISHED){
			std::cout << "game finished" << std::endl;
			break;
		}
	}
	return 0;
}
