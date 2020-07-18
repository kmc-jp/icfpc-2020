#include <iostream>
#include "lexer.hpp"
#include "interpreter.hpp"

using namespace std;

string string_of_int(int x);

string parse_list(const string &s, int &idx);

string modulate(const string s);