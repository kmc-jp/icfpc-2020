#include "utility.hpp"

std::vector<std::string> read_from_img() {
    std::vector<std::string> input;
    std::string line;
    while(std::getline(std::cin, line)) {
        input.push_back(std::move(line));
    }
    return input;
}
