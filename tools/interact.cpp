#include "../interpreter/interpreter.hpp"
#include "../interpreter/lexer.hpp"
#include "demodulator.hpp"
#include "modulator.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

std::string string_of_varid(int var_id) {
  return ":" + std::to_string(var_id);
}

void plot(std::string data) {
  std::vector<std::string> str(1);
  for (int i = 0; i < data.size(); i++) {
    if (data[i] == ' ') {
      str.push_back("");
    } else if (data[i] != '[' && data[i] != ']' && data[i] != ',') {
      str.back().push_back(data[i]);
    }
  }
  if(str.empty() || str.back().substr(0, 3) != "Nil") return;
  std::vector<std::vector<pair<int, int>>> ps(1);
  int minx = 0, maxx = 0, miny = 0, maxy = 0;
  for (int i = 0; i < str.size(); i++) {
    if (str[i].substr(0, 3) == "Nil" || str[i + 1].substr(0, 3) == "Nil") {
      ps.push_back(std::vector<pair<int, int>>());
      continue;
    }

    int x = std::stoi(str[i]);
    int y = std::stoi(str[++i]);
    ps.back().emplace_back(x, y);
    minx = std::min(minx, x);
    maxx = std::max(maxx, x);
    miny = std::min(miny, y);
    maxy = std::max(maxy, y);
  }
  vector<string> field(maxy - miny + 1, string(maxx - minx + 1, '.'));

  for (int i = ps.size() - 1; i >= 0; i--) {
    for (auto &p : ps[i]) {
      field[p.second - miny][p.first - minx] = '0' + i;
    }
  }
  field[-miny][-minx] = '+';
  std::ofstream fout("plot.txt");
  for (auto l : field) {
    fout << l << endl;
  }
  fout.close();
}
int main() {
  const std::string galaxy = string_of_varid(1338);
  Interpreter interp;
  {
    std::ifstream fin("prog.txt");
    std::string prog;
    while (std::getline(fin, prog)) {
      interp.run(std::cout, prog);
    }
    fin.close();
  }
  auto run_and_get_result_string = [&interp](const std::string &prog) {
    std::stringstream ss;
    interp.run(ss, prog);
    return ss.str();
  };
  int var_id = 2000;
  bool flag;
  std::string data;
  auto eval_and_update = [&interp, &run_and_get_result_string, &var_id, &flag, &data]() {
    auto s = run_and_get_result_string(string_of_varid(var_id));
    interp.run(std::cout, string_of_varid(var_id));
    var_id++;
    flag = s[1] == '1';
    data = run_and_get_result_string("ap car ap cdr ap cdr " + string_of_varid(var_id - 1));
    interp.run(string_of_varid(var_id) + " = ap car ap cdr " + string_of_varid(var_id - 1));
    var_id++;
  };
  interp.run(string_of_varid(var_id) + " = ap ap " + galaxy + " nil ap ap cons 0 0");
  eval_and_update();

  auto read_eval = [&](std::istream &is) {
    while (true) {
      plot(data);

      if (flag) {
        auto new_data = modulate(data);
        std::cout << "send this to aliens and input the response : " << new_data << std::endl;

        std::string res;
        is >> res;
        if (is.fail()) {
          return;
        }
        std::cout << "calculating.." << std::endl;
        auto t2 = demodulate(res);
        std::vector<Token> t1 = {{TokenType::Variable, var_id}, {TokenType::Equality, 0}, {TokenType::Apply, 0}, {TokenType::Apply, 0}, {TokenType::Variable, 1338}, {TokenType::Variable, var_id - 1}}; // :(var_id) = ap ap galaxy :(var_id -1)
        t1.insert(t1.end(), t2.begin(), t2.end());
        interp.run(t1);
        eval_and_update();
      } else {
        string res;
        getline(is, res);
        if (is.fail()) {
          return;
        }
        if (res[0] == '[') {
          auto t2 = demodulate(modulate(res));
          std::vector<Token> t1 = {{TokenType::Variable, var_id}, {TokenType::Equality, 0}};
          t1.insert(t1.end(), t2.begin(), t2.end()); // :(var_id) = token
          interp.run(t1);
          eval_and_update();
          cout << "loaded" << endl;
          getline(is, res);
        }else if (isdigit(res[0]) || res[0] == '-') {
          bool minusx = false, minusy = false;
          int x = 0, y = 0;
          int idx = 0;
          if (res[idx] == '-') minusx = true, idx++;
          for (; idx < res.size(); idx++) {
            if(!isdigit(res[idx])) break;
            x = x * 10 + (res[idx] - '0');
          }
          for (;!isdigit(res[idx]);idx++);
          if(idx > 0) if (res[idx - 1] == '-') minusy = true;
          for (; idx < res.size(); idx++) {
            if(!isdigit(res[idx])) break;
            y = y * 10 + (res[idx] - '0');
          }
          if (minusx) x = -x;
          if (minusy) y = -y;
          std::cout << "calculating.." << std::endl;
          interp.run(string_of_varid(var_id) + " = ap ap " + galaxy + " " + string_of_varid(var_id - 1) + " ap ap cons " + std::to_string(x) + " " + std::to_string(y));
          eval_and_update();
        }
      }
    }
  };
  {
  std::ifstream fin("in.txt");
  read_eval(fin);
  std::cout << "file reading mode ended" << std::endl;
  read_eval(std::cin);
  }
}
