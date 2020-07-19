#include <iostream>
#include <string>
#include "../interact/demodulator.hpp"
#include "../interact/modulator.hpp"
#include "../interpreter/token.hpp"

// binary -> ap ap ..
void debug_modulate(std::string s) {
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
        std::cerr << "Nil ";
        break;
      case TokenType::Cons:
        std::cerr << "Cons ";
        break;
      case TokenType::Number:
        std::cerr << "N[" << token.immediate << "] ";
        break;
      case TokenType::Apply:
        std::cerr << "ap ";
        break;
      default:
        std::cerr << static_cast<int>(token.type);
    }
  }
}

int main() {
  std::string s;
  while (cin >> s) {
      debug_modulate(s);
  }
}
