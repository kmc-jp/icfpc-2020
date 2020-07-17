#ifndef PARSER_HPP
#define PARSER_HPP

#include "./utility.hpp"

using ll = long long;

std::string dict_operator(const ll id) {
    if(id == 0) return "ap";
    if(id == 1) return "i";
    if(id == 2) return "t";
    if(id == 5) return "b";
    if(id == 6) return "c";
    if(id == 7) return "s";
    if(id == 8) return "f";
    if(id == 10) return "neg";
    if(id == 12) return "eq";
    if(id == 14) return "nil";
    if(id == 15) return "isnil";
    if(id == 40) return "div";
    if(id == 146) return "prod";
    if(id == 170) return "mod";
    if(id == 174) return "send";
    if(id == 341) return "dem";
    if(id == 365) return "sum";
    if(id == 401) return "dec";
    if(id == 416) return "lt";
    if(id == 417) return "inc";
    if(id == 448) return "eq";
    if(id == 64170) return "cons";
    if(id == 64171) return "cdr";
    if(id == 64174) return "car";
    if(id == 17043521) return "vec";
    return "not supported: " + std::to_string(id);
}

ll calc_number_part(std::vector<std::string> const& symbol) {
    const int n = std::min(symbol.size(), symbol[0].size()) - 1;
    ll res = 0;
    for(int y = 0; y < n; ++y) {
        for(int x = 0; x < n; ++x) {
            if(symbol[y + 1][x + 1] == '1') {
                res += 1LL << (y * n + x);
            }
        }
    }
    return res;
}

ll calc_var_number_part(std::vector<std::string> const& symbol) {
    assert(symbol.size() == symbol[0].size());
    const int n = symbol.size();
    ll res = 0;
    for(int y = 0; y < n - 3; ++y) {
        for(int x = 0; x < n - 3; ++x) {
            if(symbol[y + 2][x + 2] == '0') {
                res += 1LL << (y * (n - 3) + x);
            }
        }
    }
    return res;
}

bool is_lparen(std::vector<std::string> const& symbol) {
    if(symbol.size() != 5u || symbol[0].size() != 3u) return false;
    bool res = true;
    for(int x = 0; x < 3; ++x) {
        for(int y = 2 - x; y <= 2 + x; ++y) {
            res &= symbol[y][x] == '1';
        }
    }
    return res;
}

bool is_rparen(std::vector<std::string> const& symbol) {
    if(symbol.size() != 5u || symbol[0].size() != 3u) return false;
    bool res = true;
    for(int x = 0; x < 3; ++x) {
        for(int y = x; y <= 4 - x; ++y) {
            res &= symbol[y][x] == '1';
        }
    }
    return res;
}

bool is_list_sep(std::vector<std::string> const& symbol) {
    if(symbol.size() != 5u || symbol[0].size() != 2u) return false;
    bool res = true;
    for(auto const& s : symbol) {
        for(auto const c : s) {
            res &= c == '1';
        }
    }
    return res;
}

bool is_isnil(std::vector<std::string> const& symbol) {
    if(symbol.size() != 3u || symbol[0].size() != 3u) return false;
    bool res = true;
    const int n = symbol.size();
    for(int i = 0; i < n; ++i) {
        res &= symbol[0][i] == '1' && symbol[i][0] == '1' && symbol[n - 1][i] == '1' && symbol[i][n - 1] == '1';
    }
    return res;
}

bool is_variable(std::vector<std::string> const& symbol) {
    if(symbol.size() != symbol[0].size()) return false;
    if(symbol.size() < 4u) return false;
    if(symbol[1][1] == '0') return false;
    const int n = symbol.size();
    for(int i = 2; i < n - 1; ++i) {
        if(symbol[1][i] == '1' || symbol[i][1] == '1') return false;
    }
    bool res = true;
    for(int i = 0; i < n; ++i) {
        res &= symbol[0][i] == '1' && symbol[i][0] == '1' && symbol[n - 1][i] == '1' && symbol[i][n - 1] == '1';
    }
    return res;
}

void parse(std::vector<std::string> const& symbol) {
    if(is_lparen(symbol)) {
        std::cout << "lparen" << std::endl;
    } else if(is_rparen(symbol)) {
        std::cout << "rparen" << std::endl;
    } else if(is_list_sep(symbol)) {
        std::cout << "list_sep" << std::endl;
    } else if(is_variable(symbol)) {
        const ll id = calc_var_number_part(symbol);
        std::cout << ("x" + std::to_string(id)) << std::endl;
        //throw std::runtime_error("not supported");
    } else if(symbol[0][0] == '0') { // number
        const ll num = calc_number_part(symbol);
        if(symbol.size() == symbol[0].size()) { // positive
            std::cout << num << std::endl;
        } else {
            std::cout << -num << std::endl;
        }
    } else if(symbol[0][0] == '1') { // operator
        const ll num = calc_number_part(symbol);
        std::cout << dict_operator(num) << std::endl;
    }
}

#endif
