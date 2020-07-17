#ifndef PARSER_HPP
#define PARSER_HPP

#include "./utility.hpp"
#include "./token.hpp"

using ll = long long;

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

Token dict_operator(const ll id) {
    if(id == 0) return app;
    if(id == 1) return {TokenType::I, 0};
    if(id == 2) return {TokenType::True, 0};
    if(id == 5) return comb_b;
    if(id == 6) return comb_c;
    if(id == 7) return {TokenType::S, 0};
    if(id == 8) return {TokenType::False, 0};
    if(id == 10) return {TokenType::Negate, 0};
    if(id == 12) return {TokenType::Equality, 0};
    if(id == 14) return nil;
    if(id == 15) return "isnil";
    if(id == 40) return {TokenType::Division, 0};
    if(id == 146) return {TokenType::Product, 0};
    if(id == 170) return {TokenType::Modulate, 0};
    if(id == 174) return {TokenType::Send, 0};
    if(id == 341) return {TokenType::Demod, 0};
    if(id == 365) return sum;
    if(id == 401) return {TokenType::Pred, 0};
    if(id == 416) return {TokenType::Lt, 0};
    if(id == 417) return {TokenType::Succ, 0};
    if(id == 448) return {TokenType::Eq, 0};
    if(id == 64170) return cons;
    if(id == 64171) return {TokenType::Cdr, 0};
    if(id == 64174) return {TokenType::Car, 0};
    if(id == 17043521) return cons;

    throw std::runtime_error("dict_operator: not supported " + std::to_string(id));
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

Token tokenize_one(std::vector<std::string> const& symbol) {
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
        return dict_operator(calc_number_part(symbol));
    }
    throw std::runtime_error("tokenize_one: not implmented");
}

std::vector<Token> tokenize(std::vector<std::string> const& input) {
    std::vector<Token> res;
    for(auto&& v : trim_input(input)) {
        res.push_back(tokenize_one(std::move(v)));
    }
    return res;
}

#endif
