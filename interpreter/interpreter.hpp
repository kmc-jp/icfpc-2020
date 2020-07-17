#include <future>
#include <memory>
#include <vector>
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

//FuncApplyPtr make_apply(const Token& ins, const std::vector<FuncApplyPtr>& operands);
ApplyPtr parse(const std::vector<Token>& tokens);
void dump(const ApplyPtr& ap);

//bool is_finished(const ApplyPtr& ap);
ApplyPtr apply(const ApplyPtr& ap);
