#include <iostream>
#include "lexer.hpp"
#include "interpreter.hpp"

int main() {
  //std::vector<Token> tokens = {
  //  app, app, app, comb_s, app, app, comb_c, app, eq, number(0), number(1), app, app, comb_b, app, mul, number(2), app, app, comb_b, pwr2, app, sum, number(-1), number(1)
  //};
  //std::vector<Token> tokens = {
  //  app, app, mul, number(2), app ,app, app, comb_s, app, app, comb_c, app, eq, number(0), number(1), app, app, comb_b, app, mul, number(2), app, app, comb_b, pwr2, app, sum, number(-1), app, app, sum, number(-1), number(1)
  //};
  const std::string prog = "ap ap ap s ap ap c ap eq 0 1 ap ap b ap mul 2 ap ap b pwr2 ap add -1 1";
  const auto tokens = tokenize(prog);
  auto tree = parse(tokens);
  dump(tree);
  tree = apply(tree);
  std::cout << tree->ins.immediate << std::endl;
  return 0;
}
