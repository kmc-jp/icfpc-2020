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

/*
int main() {
  std::string s;
  while (std::cin >> s) {
    auto tokens = demodulate(s);
    for (auto token : tokens) {
      switch (token.type) {
        case TokenType::Sum:
          std::cerr << "+";
          break;
        case TokenType::Product:
          std::cerr << "*";
          break;
        case TokenType::Eq:
          std::cerr << "==";
          break;
        case TokenType::S:
          std::cerr << "S";
          break;
        case TokenType::B:
          std::cerr << "B";
          break;
        case TokenType::C:
          std::cerr << "C";
          break;
        case TokenType::Pwr2:
          std::cerr << "Pwr2";
          break;
        case TokenType::I:
          std::cerr << "I";
          break;
        case TokenType::Nil:
          std::cerr << "Nil";
          break;
        case TokenType::Cons:
          std::cerr << "Cons";
          break;
        case TokenType::Number:
          std::cerr << "N[" << token.immediate << "]";
          break;
        default:
          std::cerr << static_cast<int>(token.type);
      }
    }
  }
}
*/
