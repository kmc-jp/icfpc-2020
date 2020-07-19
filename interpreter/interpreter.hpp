#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <future>
#include <memory>
#include <ostream>
#include <vector>
#include <unordered_map>
#include "token.hpp"


struct Apply {
  virtual bool is_apply() const { return false; }
  virtual bool is_partial() const { return false; }
  virtual bool is_cons_pair() const { return false; }
  virtual bool is_object() const { return false; }
};

using ApplyPtr = std::shared_ptr<Apply>;

struct Object : Apply {
  Token ins;
  explicit Object(const Token& ins);
  bool is_object() const override { return true; }
};

struct ApplyPair : Apply {
  ApplyPtr lhs, rhs;
  ApplyPair(const ApplyPtr& lhs,
      const ApplyPtr& rhs);
  bool is_apply() const override { return true; }
};

struct Partial : Apply {
  std::function<ApplyPtr(const ApplyPtr&)> func;
  std::string func_name;
  Partial(std::function<ApplyPtr(const ApplyPtr&)> func, const std::string& func_name);
  bool is_partial() const override { return true; }
};

struct ConsPair : Apply {
  ApplyPtr car, cdr;
  ConsPair(const ApplyPtr& car,
      const ApplyPtr& cdr);
  bool is_cons_pair() const override { return true; }
};

using Environment = std::unordered_map<uint64_t, ApplyPtr>;

//FuncApplyPtr make_apply(const Token& ins, const std::vector<FuncApplyPtr>& operands);
ApplyPtr parse(const std::vector<Token>& tokens);
int64_t get_int(const ApplyPtr& ap);

//bool is_finished(const ApplyPtr& ap);
ApplyPtr eval(const ApplyPtr& ap, const std::shared_ptr<Environment>& env);


class Interpreter {
public:
  Interpreter();

  void run(std::ostream& os, const std::string& prog);
  void run(const std::string& prog);
  void run(std::ostream& os, const std::vector<Token>& tokens);
  void run(const std::vector<Token>& tokens);

private:
  std::shared_ptr<Environment> env;
};

#endif
