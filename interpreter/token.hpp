#include <string>

enum class TokenType {
  Number,
  Equality,
  Succ,
  Pred,
  Variable,
  Sum,
  Product,
  Division,
  Eq,
  Lt,
  Modulate,
  Demod,
  Send,
  Negate,
  Apply,
  S,
  C,
  B,
  True,
  False,
  Pwr2,
  I,
  Cons,
  Car,
  Cdr,
  Nil,
  Partial
};

struct Token {
  TokenType type;
  int64_t immediate;
};

constexpr Token app = {TokenType::Apply, 0};
constexpr Token sum = {TokenType::Sum, 0};
constexpr Token comb_c = {TokenType::C, 0};
constexpr Token comb_b = {TokenType::C, 0};
constexpr Token cons = {TokenType::Cons, 0};
constexpr Token nil = {TokenType::Nil, 0};
constexpr Token number(int64_t x) { return {TokenType::Number, x}; }
