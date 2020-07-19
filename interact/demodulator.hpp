#include "../interpreter/token.hpp"
#include <vector>

std::vector<Token> demodulate(const std::string &s);

enum class demodataEnum{
  demoInt,
  demoList,
};


struct demodata{
	demodataEnum type;
	int num;
	std::vector<struct demodata> vec;
};

struct demodata demodulateList(const std::string &s);
