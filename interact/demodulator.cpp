#include "demodulator.hpp"

std::pair<std::vector<Token>, int> demodulate(const std::string &num, int i) {
  if (num.substr(i, 2) == "00") {
    return {{nil}, 2};
  }
  if (num.substr(i, 2) == "11") {
    auto result1 = demodulate(num, i + 2);
    auto result2 = demodulate(num, i + 2 + result1.second);
    std::vector<Token> tokens = {app, app, cons};
    tokens.insert(tokens.end(), result1.first.begin(), result1.first.end());
    tokens.insert(tokens.end(), result2.first.begin(), result2.first.end());
    return {tokens, result1.second + result2.second + 2};
  }
  int64_t result = 0;
  if (num.substr(i, 2) == "01") {
    result = 1;
  } else {
    result = -1;
  }
  int n = 0;
  for (int pos = i + 2; pos < (int)num.size(); pos++) {
    if (num[pos] == '0') {
      n = pos - i - 2;
      break;
    }
  }
  int64_t val = 0;
  for (int j = 0; j < 4 * n; j++) {
    val *= 2;
    if (num[i + 2 + n + 1 + j] == '1') {
      val += 1;
    }
  }
  result *= val;
  Token token = {TokenType::Number, result};
  return {{token}, 2 + 4 * n + n + 1};
}

std::vector<Token> demodulate(const std::string &s) {
  auto result = demodulate(s, 0);
  return result.first;
}

std::pair<struct AlienData, int> demodulateList(const std::string &num, int i) {
  if (num.substr(i, 2) == "00") {
    struct AlienData data;
    data.type = AlienDataType::List;
    return {data, 2};
  }
  if (num.substr(i, 2) == "11") {
    auto result1 = demodulateList(num, i + 2);
    auto result2 = demodulateList(num, i + 2 + result1.second);
    if(result2.first.type == AlienDataType::List){
      result2.first.vec.insert(result2.first.vec.begin(),result1.first);
      return {result2.first, result1.second + result2.second + 2};
    }
    else{
      struct AlienData data;
      data.type = AlienDataType::List;
      data.vec.push_back(result1.first);
      data.vec.push_back(result2.first);
      return {data, result1.second + result2.second + 2};
    }
  }
  int64_t result = 0;
  if (num.substr(i, 2) == "01") {
    result = 1;
  } else {
    result = -1;
  }
  int n = 0;
  for (int pos = i + 2; pos < (int)num.size(); pos++) {
    if (num[pos] == '0') {
      n = pos - i - 2;
      break;
    }
  }
  int64_t val = 0;
  for (int j = 0; j < 4 * n; j++) {
    val *= 2;
    if (num[i + 2 + n + 1 + j] == '1') {
      val += 1;
    }
  }
  result *= val;
  struct AlienData data;
  data.type = AlienDataType::Int;
  data.num = result;
  
  return {data, 2 + 4 * n + n + 1};
}

struct AlienData demodulateList(const std::string &s) {
  auto result = demodulateList(s, 0);
  return result.first;
}

std::string string_of_alien_data(const AlienData& data) {
  std::string res;
  if(data.type == AlienDataType::Int) {
    res = std::to_string(data.num);
  } else if(data.type == AlienDataType::List) {
    res = "[";
    for(int i = 0; i < (int)data.vec.size(); ++i) {
      if(i != 0) res += ", ";
      res += string_of_alien_data(data.vec[i]);
    }
    res += "]";
  }
  return res;
}
