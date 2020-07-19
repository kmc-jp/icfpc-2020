#include "command.hpp"

Accelerate::Accelerate(int64_t ship_id_, point const& vec_)
  : ship_id(ship_id_), vec(vec_)
{}

Detonate::Detonate(int64_t ship_id_)
  : ship_id(ship_id_)
{}

Shoot::Shoot(int64_t ship_id_, point const& target_, int64_t x3_)
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


CommandPtr make_accelerate(int64_t ship_id, point const& vec) {
  return std::make_shared<Accelerate>(ship_id, vec);
}

CommandPtr make_detonate(int64_t ship_id) {
  return std::make_shared<Detonate>(ship_id);
}

CommandPtr make_shoot(int64_t ship_id, point const& target, int64_t x3) {
  return std::make_shared<Shoot>(ship_id, target, x3);
}


std::string make_command_query(std::string const& player_key, std::vector<CommandPtr> const& commands) {
  std::string s;
  s += "[4";
  s += ", " + player_key;
  s += ", ";
  if(commands.empty()) {
    s += "Nil";
  } else {
    s += "[";
    for(int i = 0; i < (int)commands.size(); ++i) {
      if(i == 0) s += ", ";
      s += commands[i]->to_list_string();
    }
    s += ", Nil]";
  }
  s += ", Nil]";
  return s;
}
