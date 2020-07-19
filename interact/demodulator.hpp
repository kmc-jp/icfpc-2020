#include "../interpreter/token.hpp"
#include <vector>

std::vector<Token> demodulate(const std::string &s);

enum class AlienDataType{
  Int,
  List,
};


struct AlienData{
  AlienDataType type;
  uint64_t num;
  std::vector<struct AlienData> vec;
};

struct AlienData demodulateList(const std::string &s);
