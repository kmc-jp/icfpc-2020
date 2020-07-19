#include "../interpreter/token.hpp"
#include <vector>

std::vector<Token> demodulate(const std::string &s);

enum class AlianDataType{
  Int,
  List,
};


struct AlianData{
  AlianDataType type;
  int num;
  std::vector<struct AlianData> vec;
};

struct AlianData demodulateList(const std::string &s);
