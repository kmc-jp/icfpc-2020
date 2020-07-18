#ifndef LEXER_HPP
#define LEXER_HPP

#include "./utility.hpp"
#include "./token.hpp"

using ll = long long;

auto trim_input(std::vector<std::string> const& in);

Token tokenize_one(std::vector<std::string> const& symbol);
std::vector<Token> tokenize(std::vector<std::string> const& input);

// require: all tokens are separated by spaces
std::vector<Token> tokenize(std::string const& input);

#endif
