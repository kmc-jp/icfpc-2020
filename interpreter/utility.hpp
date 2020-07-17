#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <algorithm>

std::vector<std::string> read_from_stdin() {
    std::vector<std::string> input;
    std::string line;
    while(std::getline(std::cin, line)) {
        input.push_back(std::move(line));
    }
    return input;
}

#endif
