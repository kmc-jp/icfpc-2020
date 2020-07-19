#ifndef POINT_HPP
#define POINT_HPP

#include <string>

struct point {
  int64_t x, y;

  // todo: Nil ?
    std::string to_string() const {
      return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
  std::string to_list_string() const {
    return "[" + std::to_string(x) + ", " + std::to_string(y) + ", Nil]";
  }
};

#endif
