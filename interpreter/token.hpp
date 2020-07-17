#include <string>

enum class TokenType {
  Number, // 0
  Equality,
  Succ,
  Pred,
  Variable,
  Sum, // 5
  Product,
  Division,
  Eq,
  Lt,
  Modulate, // 10
  Demod,
  Send,
  Negate,
  Apply,
  S, // 15
  C,
  B,
  True,
  False,
  Pwr2, // 20
  I,
  Cons,
  Car,
  Cdr,
  Nil, // 25
  IsNil,
  Partial
};

struct Token {
  TokenType type;
  int64_t immediate;
};

constexpr Token app = {TokenType::Apply, 0};
constexpr Token inc = {TokenType::Succ, 0};
constexpr Token dec = {TokenType::Pred, 0};
constexpr Token sum = {TokenType::Sum, 0};
constexpr Token mul = {TokenType::Product, 0};
constexpr Token eq = {TokenType::Eq, 0};
constexpr Token comb_s = {TokenType::S, 0};
constexpr Token comb_c = {TokenType::C, 0};
constexpr Token comb_b = {TokenType::B, 0};
constexpr Token pwr2 = {TokenType::Pwr2, 0};
constexpr Token cons = {TokenType::Cons, 0};
constexpr Token nil = {TokenType::Nil, 0};
constexpr Token number(int64_t x) { return {TokenType::Number, x}; }
