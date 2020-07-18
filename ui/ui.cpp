#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include "../interpreter/lexer.hpp"
#include "../interpreter/interpreter.hpp"
#include "../demodulator/demodulator.hpp"
#include "../modulator/modulator.hpp"

using namespace std;

int main(){
    Interpreter interp;

    std::ifstream fin("../interpreter/test/interpreter/galaxy.txt");
    std::string prog;
    while(std::getline(fin, prog)){
        interp.run(std::cout, prog);
    }
    interp.run(std::cout, ":1999 = ap ap :1338 nil ap ap cons 0 0");
    int var_id = 2000;
    std::stringstream ss;
    std::string ls;
    while(true){
        std::getline(std::cin, prog);
        if(prog == "quit;;") break;
        interp.run(std::cout,
                   ":" + std::to_string(var_id) + " = ap ap :1338 :" + std::to_string(var_id - 1) + " ap ap cons " +
                   prog);
        interp.run(ss, ":" + std::to_string(var_id));
        std::getline(ss, ls);
        std::cout << ls << std::endl;
        var_id++;
        interp.run(std::cout,
                   ":" + std::to_string(var_id) + " = ap car ap cdr :" + std::to_string(var_id - 1));
        var_id++;
        string data = "";
        int count = 0;
        int depth = 0;
        for(int i = 0; i < ls.size(); i++){
            if(ls[i] == '['){
                depth++;
            }else if(ls[i] == ']'){
                depth--;
                if(depth == 1){
                    data = ls.substr(i + 2);
                    break;
                }
            }
        }
        while(ls[1] == '1'){
            cout << data << endl;
            data = data.substr(1, data.size() - 7);
            cout << modulate(data) << endl;
            cin >> data;
            vector <Token> tk = demodulate(data);
            data = "";
            for(auto t: tk){
                switch(t.type){
                    case TokenType::Number:
                        data += " " + to_string(t.immediate);
                        break;
                    case TokenType::Cons:
                        data += " cons";
                        break;
                    case TokenType::Nil:
                        data += " nil";
                        break;
                    case TokenType::Apply:
                        data += " ap";
                        break;
                    default:
                        break;
                }
            }
            cout << data << endl;
            cout << ":" + std::to_string(var_id) + " = ap ap :1338 :" + std::to_string(var_id - 1) + data +";" << endl;
            interp.run(cout,
                       ":" + std::to_string(var_id) + " = ap ap :1338 :" + std::to_string(var_id - 1) + data);
            interp.run(ss, ":" + std::to_string(var_id));
            std::getline(ss, ls);
            var_id++;
            interp.run(std::cout,
                       ":" + std::to_string(var_id) + " = ap car ap cdr :" + std::to_string(var_id - 1) + "\n");
            var_id++;
        }
        std::vector <std::string> str;
        for(int i = 0; i < data.size(); i++){
            if(data[i] == ' '){
                str.push_back("");
            }else if(data[i] != '[' && data[i] != ']' && data[i] != ','){
                str.back().push_back(data[i]);
            }
        }
        std::vector < std::vector < pair < int, int >> > ps(1);
        int minx = 0, maxx = 0, miny = 0, maxy = 0;
        for(int i = 0; i < str.size(); i++){
            if(str[i] == "Nil"){
                ps.push_back(std::vector < pair < int, int >> ());
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
        vector <string> field(maxy - miny + 1, string(maxx - minx + 1, '.'));

        for(int i = ps.size() - 1; i >= 0; i--){
            for(auto &p : ps[i]){
                field[p.second - miny][p.first - minx] = '0' + i;
            }
        }
        field[-miny][-minx] = '+';
        std::ofstream fout("out.txt");
        for(auto l:field){
            fout << l << endl;
        }
        fout.close();
    }
    return 0;
}
