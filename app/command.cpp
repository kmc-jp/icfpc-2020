#include "command.hpp"

Accelerate::Accelerate(uint64_t ship_id_, point const& vec_)
  : ship_id(ship_id_), vec(vec_)
{}

Detonate::Detonate(uint64_t ship_id_)
  : ship_id(ship_id_)
{}

Shoot::Shoot(uint64_t ship_id_, point const& target_, uint64_t x3_)
  : ship_id(ship_id_), x3(x3_), target(target_)
{}

std::string Accelerate::to_list_string() const {
  std::string s;
  s += "[" + std::to_string(static_cast<int>(command_type()));
  s += ", " + std::to_string(ship_id);
  s += ", " + vec.to_list_string();
  s += ", Nil]";
  return s;
}

std::string Detonate::to_list_string() const {
  std::string s;
  s += "[" + std::to_string(static_cast<int>(command_type()));
  s += ", " + std::to_string(ship_id);
  s += ", Nil]";
  return s;
}

std::string Shoot::to_list_string() const {
  std::string s;
  s += "[" + std::to_string(static_cast<int>(command_type()));
  s += ", " + std::to_string(ship_id);
  s += ", " + target.to_list_string();
  s += ", " + std::to_string(x3);
  s += ", Nil]";
  return s;
}


CommandPtr make_accelerate(uint64_t ship_id, point const& vec) {
  return std::make_shared<Accelerate>(ship_id, vec);
}

CommandPtr make_detonate(uint64_t ship_id) {
  return std::make_shared<Detonate>(ship_id);
}

CommandPtr make_shoot(uint64_t ship_id, point const& target, uint64_t x3) {
  return std::make_shared<Shoot>(ship_id, target, x3);
}
