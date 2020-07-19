#include "../command.hpp"
#include <iostream>
#include <cassert>

int main() {
    const auto acc = make_accelerate(1, {2, 3});
    const auto det = make_detonate(2);
    const auto sht = make_shoot(3, {4, 5}, 6);

    const auto acc_s = acc->to_list_string();
    const auto det_s = det->to_list_string();
    const auto sht_s = sht->to_list_string();

    std::cout << "acc: " << acc_s << std::endl;
    assert(acc_s == "[0, 1, [2, 3, Nil], Nil]");
    std::cout << "det: " << det_s << std::endl;
    assert(det_s == "[1, 2, Nil]");
    std::cout << "sht: " << sht_s << std::endl;
    assert(sht_s == "[2, 3, [4, 5, Nil], 6, Nil]");

}
