#ifndef DEMODULATOR_HPP
#define DEMODULATOR_HPP

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

std::string string_of_alien_data(const AlienData& data);

#endif
