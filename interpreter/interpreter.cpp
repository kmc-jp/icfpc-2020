#include <iostream>
#include <stdexcept>
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
      default: std::cerr << static_cast<int>(ap->ins.type);
    }
  }
}

void dump(const ApplyPtr& ap) {
  dump(ap, 0);
  std::cerr << std::endl;
}

ApplyPtr apply(const ApplyPtr& ap) {
  if (ap->is_apply()) {
    auto op = apply(ap->lhs);
    switch (op->ins.type) {
      case TokenType::Succ:
        {
          auto num = apply(ap->rhs)->ins.immediate;
          return make_apply(number(num + 1));
        }
      case TokenType::Pred:
        {
          auto num = apply(ap->rhs)->ins.immediate;
          return make_apply(number(num - 1));
        }
      case TokenType::Sum:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
            auto lval = apply(lhs)->ins.immediate;
            auto rval = apply(rhs)->ins.immediate;
            return make_apply(number(lval + rval));
          });
        }
      case TokenType::Product:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
            auto lval = apply(lhs)->ins.immediate;
            auto rval = apply(rhs)->ins.immediate;
            return make_apply(number(lval * rval));
          });
        }
      case TokenType::Eq:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
            auto lval = apply(lhs)->ins.immediate;
            auto rval = apply(rhs)->ins.immediate;
            return make_apply({lval == rval ? TokenType::True : TokenType::False, 0});
          });
        }
      case TokenType::Lt:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
            auto lval = apply(lhs)->ins.immediate;
            auto rval = apply(rhs)->ins.immediate;
            return make_apply({lval < rval ? TokenType::True : TokenType::False, 0});
          });
        }
      case TokenType::S:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& mhs) -> ApplyPtr {
            return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              return apply(make_apply(make_apply(lhs, rhs), apply(make_apply(mhs, rhs))));
            });
          });
        }
      case TokenType::C:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& mhs) -> ApplyPtr {
            return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              return apply(make_apply(make_apply(lhs, rhs), mhs));
            });
          });
        }
      case TokenType::B:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& mhs) -> ApplyPtr {
            return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              return apply(make_apply(lhs, apply(make_apply(mhs, rhs))));
            });
          });
        }
      case TokenType::True:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
            return apply(lhs);
          });
        }
      case TokenType::False:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
            return apply(rhs);
          });
        }
      case TokenType::Pwr2:
        {
          auto num = apply(ap->rhs)->ins.immediate;
          return make_apply(number(INT64_C(1) << num));
        }
    case TokenType::I:
        {
          return apply(ap->rhs);
        }
      case TokenType::Cons:
        {
          auto lhs = ap->rhs;
          return make_apply([=] (const ApplyPtr& mhs) -> ApplyPtr {
            return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              return apply(make_apply(make_apply(rhs, lhs), mhs));
            });
          });
        }
      case TokenType::Nil:
        return make_apply({TokenType::True, 0});
      case TokenType::Partial:
        return op->func(ap->rhs);
      default:
        throw std::runtime_error("BAD apply: ins type is " + std::to_string(static_cast<int>(op->ins.type)));
    }
  } else {
    return ap;
  }
}
