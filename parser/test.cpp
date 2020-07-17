#include "./utility.hpp"
#include "./parser.hpp"

int main() {
    auto input = read_from_stdin();
    auto trimed = trim_input(input);

    for(auto const& v : trimed) {
        parse(v);
    }
}
