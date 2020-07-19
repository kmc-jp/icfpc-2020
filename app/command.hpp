#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <vector>
#include <memory>
#include <stdexcept>
#include "point2d.hpp"

enum class CommandType {
  Accelerate,
  Detonate,
  Shoot
};

struct Command {
  virtual ~Command() {}

  virtual CommandType command_type() const = 0;
  virtual bool is_accelerate() const { return false; }
  virtual bool is_detonate() const { return false; }
  virtual bool is_shoot() const { return false; }

  virtual std::string to_list_string() const = 0;
};

using CommandPtr = std::shared_ptr<Command>;

struct Accelerate : public Command {
  uint64_t ship_id;
  point vec;

  Accelerate(uint64_t ship_id, point const& vec);

  CommandType command_type() const override { return CommandType::Accelerate; }
  bool is_accelerate() const override { return true; }

  std::string to_list_string() const override;
};

struct Detonate : public Command {
  uint64_t ship_id;

  Detonate(uint64_t ship_id);

  CommandType command_type() const override { return CommandType::Detonate; }
  bool is_detonate() const override { return true; }

  std::string to_list_string() const override;
};

struct Shoot : public Command {
  uint64_t ship_id, x3; // todo: x3
  point target;

  Shoot(uint64_t ship_id, point const& target, uint64_t x3);

  CommandType command_type() const override { return CommandType::Shoot; }
  bool is_shoot() const override { return true; }

  std::string to_list_string() const override;
};


CommandPtr make_accelerate(uint64_t ship_id, point const& vec);
CommandPtr make_detonate(uint64_t ship_id);
CommandPtr make_shoot(uint64_t ship_id, point const& target, uint64_t x3); // todo: x3


#endif
