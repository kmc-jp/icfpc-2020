#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <future>
#include <memory>
#include <vector>
#include <unordered_map>
#include "token.hpp"


struct Apply {
  Token ins;
  std::function<std::shared_ptr<Apply>(const std::shared_ptr<Apply>&)> func;
  std::shared_ptr<Apply> lhs, rhs;
  explicit Apply(const Token& ins);
  explicit Apply(std::function<std::shared_ptr<Apply>(const std::shared_ptr<Apply>&)> func);
  Apply(const std::shared_ptr<Apply>& lhs,
      const std::shared_ptr<Apply>& rhs);
  bool is_apply() const { return ins.type == TokenType::Apply; }
  bool is_partial() const { return ins.type == TokenType::Partial; }
};

using ApplyPtr = std::shared_ptr<Apply>;

using Environment = std::unordered_map<uint64_t, ApplyPtr>;

//FuncApplyPtr make_apply(const Token& ins, const std::vector<FuncApplyPtr>& operands);
ApplyPtr parse(const std::vector<Token>& tokens);
void dump(const ApplyPtr& ap);

//bool is_finished(const ApplyPtr& ap);
ApplyPtr eval(const ApplyPtr& ap, const std::shared_ptr<Environment>& env);



class Interpreter {
public:
  Interpreter();

  void run(const std::string& prog);
  void run(const std::vector<Token>& tokens);

private:
  std::shared_ptr<Environment> env;
};

#endif
