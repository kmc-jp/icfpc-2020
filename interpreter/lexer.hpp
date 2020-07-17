#ifndef PARSER_HPP
#define PARSER_HPP

#include "./utility.hpp"
#include "./token.hpp"

using ll = long long;

void skip_spaces(std::string const& in, int& pos) {
    while(pos < (int)in.size() && in[pos] == ' ') ++pos;
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
    if(id == 15) return  {TokenType::IsNil, 0};
    if(id == 40) return  {TokenType::Division, 0};
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
        throw std::runtime_error("tokenize_one: not implemented lparen");
    } else if(is_rparen(symbol)) {
        throw std::runtime_error("tokenize_one: not implemented rparen");
    } else if(is_list_sep(symbol)) {
        throw std::runtime_error("tokenize_one: not implemented list_sep");
    } else if(is_variable(symbol)) {
        const ll id = calc_var_number_part(symbol);
        std::cout << ("x" + std::to_string(id)) << std::endl;
        //throw std::runtime_error("not supported");
    } else if(symbol[0][0] == '0') { // number
        const ll num = calc_number_part(symbol);
        if(symbol.size() == symbol[0].size()) { // positive
            return number(num);
        } else {
            return number(-num);
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

bool is_number(std::string const& s) {
    bool res = true;
    for(int i = s[0] == '-'; i < (int)s.size(); ++i) {
        res &= isdigit(s[i]);
    }
    return res;
}

// require: all tokens are separated by spaces
std::vector<Token> tokenize(std::string const& input) {
    std::vector<Token> res;
    std::string cur;
    int pos = 0;
    while(true) {
        skip_spaces(input, pos);
        if(pos >= (int)input.size()) return res;

        while(pos < (int)input.size() && input[pos] != ' ') cur += input[pos++];

        if(cur == "ap")            res.push_back(app);
        else if(is_number(cur))    res.push_back(number(std::stoll(cur)));
        else if(cur == "inc")      res.push_back({TokenType::Succ, 0});
        else if(cur == "dec")      res.push_back({TokenType::Pred, 0});
        else if(cur == "add")      res.push_back(sum);
        else if(cur == "=")        res.push_back({TokenType::Equality, 0});
        else if(cur[0] == 'x')     res.push_back({TokenType::Variable, std::stoll(cur.substr(1))});
        else if(cur == "mul")      res.push_back({TokenType::Product, 0});
        else if(cur == "div")      res.push_back({TokenType::Division, 0});
        else if(cur == "eq")       res.push_back({TokenType::Eq, 0});
        else if(cur == "lt")       res.push_back({TokenType::Lt, 0});
        else if(cur == "mod")      res.push_back({TokenType::Modulate, 0});
        else if(cur == "dem")      res.push_back({TokenType::Demod, 0});
        else if(cur == "send")     res.push_back({TokenType::Send, 0});
        else if(cur == "neg")      res.push_back({TokenType::Negate, 0});
        else if(cur == "s")        res.push_back({TokenType::S, 0});
        else if(cur == "c")        res.push_back({TokenType::C, 0});
        else if(cur == "b")        res.push_back({TokenType::B, 0});
        else if(cur == "t")        res.push_back({TokenType::True, 0});
        else if(cur == "f")        res.push_back({TokenType::False, 0});
        else if(cur == "i")        res.push_back({TokenType::I, 0});
        else if(cur == "pwr2")     res.push_back({TokenType::Pwr2, 0}); // ???
        else if(cur == "cons")     res.push_back(cons);
        else if(cur == "car")      res.push_back({TokenType::Car, 0});
        else if(cur == "cdr")      res.push_back({TokenType::Cdr, 0});
        else if(cur == "nil")      res.push_back(nil);
        else if(cur == "isnil")    res.push_back({TokenType::IsNil, 0});
        else if(cur == "(")        throw std::runtime_error("not implemented");
        else if(cur == ",")        throw std::runtime_error("not implemented");
        else if(cur == ")")        throw std::runtime_error("not implemented");
        else if(cur == "vec")      res.push_back(cons);
        else if(cur == "draw")     throw std::runtime_error("not implemented draw");
        else if(cur == "interact") throw std::runtime_error("not implemented: interact");
        else                       throw std::runtime_error("error lexing: " + cur);

        cur.clear();
    }
}

#endif
