#include<cassert>
#include "response.hpp"


GameResponse getGameResponse(AlienData data){
	GameResponse res;
	assert(data.type == AlienDataType::List);
	assert(data.vec.size() >= 1);
	res.status = data.vec[0].num;
	if(res.status == 0)return res;
	assert(data.vec.size() == 4);
	res.gameStage = data.vec[1].num;
	assert(data.vec[2].type == AlienDataType::List);
	assert(data.vec[2].vec.size() == 4);
	for(int i = 0; i < 4; i++){
		res.staticGameInfo.push_back(data.vec[2].vec[i].num);
	}
	return res;
}
