#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <vector>
#include <memory>
#include <stdexcept>

struct Command {
    virtual ~Command();

    virtual uint64_t command_id() const = 0;
    virtual bool is_accelerate() const { return false; }
    virtual bool is_detonate() const { return false; }
    virtual bool is_shoot() const { return false; }
};

using CommandPtr = std::shared_ptr<Command>;

struct Accelerate : public Command {
    uint64_t ship_id;
    std::vector<uint64_t> vec;

    uint64_t command_id() const override { return 0; }
    bool is_accelerate() const override { return true; }
};

struct Detonate : public Command {
    uint64_t ship_id;

    uint64_t command_id() const override { return 1; }
    bool is_detonate() const override { return true; }
};

struct Shoot : public Command {
    uint64_t ship_id, target, x3; // todo: x3

    uint64_t command_id() const override { return 2; }
    bool is_shoot() const override { return true; }
};

#endif
