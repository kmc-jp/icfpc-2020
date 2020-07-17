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

void skip_spaces(std::vector<std::string> const& in, int& x) {
    while(x < (int)in[0].size()) {
        bool all_space = true;
        for(int y = 0; y < (int)in.size(); ++y) {
            all_space &= in[y][x] == '0';
        }
        if(!all_space) break;
        ++x;
    }
}

auto trim_input(std::vector<std::string> const& in) {
    int x = 0;
    std::vector<std::vector<std::string>> res;
    while(true) {
        skip_spaces(in, x);
        if(x == (int)in[0].size()) break;

        std::vector<std::string> tmp(in.size());
        // scanning
        while(x < (int)in[0].size()) {
            bool exists_one = false;
            for(int y = 0; y < (int)in.size(); ++y) {
                exists_one |= in[y][x] == '1';
            }
            if(!exists_one) break;
            for(int y = 0; y < (int)in.size(); ++y) {
                tmp[y].push_back(in[y][x]);
            }
            ++x;
        }
        // trim top/bottom
        tmp.erase(std::remove_if(tmp.begin(), tmp.end(),
                                 [] (auto const& s) { return s.find_first_of('1') == std::string::npos; }),
                  tmp.end());
        res.push_back(std::move(tmp));
    }
    return res;
}

#endif
