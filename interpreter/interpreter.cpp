#include <iostream>
#include <stdexcept>
#include "lexer.hpp"
#include "interpreter.hpp"

auto id = [] (const ApplyPtr& ap) { return ap; };

Apply::Apply(const Token& ins) : ins(ins), func(id), lhs(), rhs() {}
Apply::Apply(std::function<ApplyPtr(const ApplyPtr&)> func)
  : ins({TokenType::Partial, 0}), func(func), lhs(), rhs() {}
Apply::Apply(const ApplyPtr& lhs, const ApplyPtr& rhs)
    : ins(app), func(id), lhs(lhs), rhs(rhs) {}

ApplyPtr make_apply(const Token& ins) {
  return std::make_shared<Apply>(ins);
}

ApplyPtr make_apply(const std::function<ApplyPtr(const ApplyPtr&)>& func) {
  return std::make_shared<Apply>(func);
}

ApplyPtr make_apply(const ApplyPtr& lhs, const ApplyPtr& rhs) {
  return std::make_shared<Apply>(lhs, rhs);
}

template <typename I>
std::pair<ApplyPtr, I> parse(I itr, I last) {
  if (itr->type != TokenType::Apply) {
    return {make_apply(*itr), std::next(itr)};
  }
  ++itr;
  auto [lhs, itr1] = parse(itr, last);
  itr = itr1;
  auto [rhs, itr2] = parse(itr, last);
  itr = itr2;
  return {make_apply(lhs, rhs), itr};
}

ApplyPtr parse(const std::vector<Token>& tokens) {
  using std::begin;
  using std::end;
  return parse(begin(tokens), end(tokens)).first;
}

void dump(const ApplyPtr& ap, int level) {
  if (ap->is_apply()) {
    std::cerr << std::string(level, '.') << "(" << std::endl;
    dump(ap->lhs, level+1);
    std::cerr << std::endl;
    dump(ap->rhs, level+1);
    std::cerr << std::endl;
    std::cerr << std::string(level, '.') << ")";
  } else if (ap->is_partial()) {
    std::cerr << std::string(level, '.') << "P";
  } else {
    std::cerr << std::string(level, '.');
    auto type = ap->ins.type;
    switch (type) {
      case TokenType::Sum: std::cerr << "+"; break;
      case TokenType::Product: std::cerr << "*"; break;
      case TokenType::Eq: std::cerr << "=="; break;
      case TokenType::S: std::cerr << "S"; break;
      case TokenType::B: std::cerr << "B"; break;
      case TokenType::C: std::cerr << "C"; break;
      case TokenType::Pwr2: std::cerr << "Pwr2"; break;
      case TokenType::I:    std::cerr << "I"; break;
      case TokenType::Nil: std::cerr << "Nil"; break;
      case TokenType::Cons: std::cerr << "Cons"; break;
      case TokenType::Number: std::cerr << "N[" << ap->ins.immediate << "]"; break;
      case TokenType::Variable: std::cerr << "Var[" << ap->ins.immediate << "]"; break;
      default: std::cerr << static_cast<int>(ap->ins.type);
    }
  }
}

void dump(const ApplyPtr& ap) {
  dump(ap, 0);
  std::cerr << std::endl;
}

ApplyPtr eval(const ApplyPtr& ap, std::shared_ptr<Environment> const& env) {
  if (ap->is_apply()) {
    auto op = eval(ap->lhs, env);
    switch (op->ins.type) {
      case TokenType::Succ:
        {
          auto num = eval(ap->rhs, env)->ins.immediate;
          return make_apply(number(num + 1));
        }
      case TokenType::Pred:
        {
          auto num = eval(ap->rhs, env)->ins.immediate;
          return make_apply(number(num - 1));
        }
      case TokenType::Sum:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
            auto lval = eval(lhs, env)->ins.immediate;
            auto rval = eval(rhs, env)->ins.immediate;
            return make_apply(number(lval + rval));
          });
        }
      case TokenType::Product:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
            auto lval = eval(lhs, env)->ins.immediate;
            auto rval = eval(rhs, env)->ins.immediate;
            return make_apply(number(lval * rval));
          });
        }
      case TokenType::Eq:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
            auto lval = eval(lhs, env)->ins.immediate;
            auto rval = eval(rhs, env)->ins.immediate;
            return make_apply({lval == rval ? TokenType::True : TokenType::False, 0});
          });
        }
      case TokenType::S:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& mhs) -> ApplyPtr {
            return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              return eval(make_apply(make_apply(lhs, rhs), eval(make_apply(mhs, rhs), env)), env);
            });
          });
        }
      case TokenType::C:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& mhs) -> ApplyPtr {
            return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              return eval(make_apply(make_apply(lhs, rhs), mhs), env);
            });
          });
        }
      case TokenType::B:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& mhs) -> ApplyPtr {
            return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              return eval(make_apply(lhs, eval(make_apply(mhs, rhs), env)), env);
            });
          });
        }
      case TokenType::True:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
            return eval(lhs, env);
          });
        }
      case TokenType::False:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
            return eval(rhs, env);
          });
        }
      case TokenType::Pwr2:
        {
          auto num = eval(ap->rhs, env)->ins.immediate;
          return make_apply(number(INT64_C(1) << num));
        }
      case TokenType::I:
        return eval(ap->rhs, env);
      case TokenType::Cons:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& mhs) -> ApplyPtr {
            return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              return eval(make_apply(make_apply(rhs, lhs), mhs), env);
            });
          });
        }
      case TokenType::Nil:
        return make_apply({TokenType::True, 0});
      case TokenType::Partial:
        return op->func(ap->rhs);
      case TokenType::Variable:
          if(env->count(ap->lhs->ins.immediate)) {
            return eval(env->at(ap->lhs->ins.immediate), env);
          } else {
            return ap; // can do nothing (stuck)
          }
      default:
        throw std::runtime_error("BAD apply: ins type is " + std::to_string(static_cast<int>(op->ins.type)));
    }
  } else {
    if(ap->ins.type == TokenType::Variable && env->count(ap->ins.immediate)) {
      return eval(env->at(ap->ins.immediate), env);
    } else {
      return ap;
    }
  }
}


Interpreter::Interpreter()
    : env(std::make_shared<Environment>())
{}

void Interpreter::run(const std::string& prog) {
    run(tokenize(prog));
}

void Interpreter::run(const std::vector<Token>& tokens) {
    if(tokens.size() >= 2u && tokens[0].type == TokenType::Variable && tokens[1].type == TokenType::Equality) { // decl
        const auto id = tokens[0].immediate;
        (*env)[id] = parse(std::vector<Token>{tokens.begin() + 2, tokens.end()});
    } else { // eval
        auto tree = parse(tokens);
        dump(tree);
        tree = eval(tree, env);
        std::cout << tree->ins.immediate << std::endl;
    }
}
