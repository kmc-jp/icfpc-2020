#include <iostream>
#include <stdexcept>
#include "lexer.hpp"
#include "interpreter.hpp"

auto id = [] (const ApplyPtr& ap) { return ap; };

Object::Object(const Token& ins) : ins(ins) {}
Partial::Partial(std::function<ApplyPtr(const ApplyPtr&)> func, const std::string& func_name)
  : func(func), func_name(func_name) {}
ApplyPair::ApplyPair(const ApplyPtr& lhs, const ApplyPtr& rhs)
    : lhs(lhs), rhs(rhs) {}
ConsPair::ConsPair(const ApplyPtr& car, const ApplyPtr& cdr)
    : car(car), cdr(cdr) {}

ApplyPtr make_apply(const Token& ins) {
  return std::make_shared<Object>(ins);
}

ApplyPtr make_apply(const std::function<ApplyPtr(const ApplyPtr&)>& func, const std::string& func_name) {
  return std::make_shared<Partial>(func, func_name);
}

ApplyPtr make_apply(const ApplyPtr& lhs, const ApplyPtr& rhs) {
  return std::make_shared<ApplyPair>(lhs, rhs);
}

ApplyPtr make_cons_pair(const ApplyPtr& car, const ApplyPtr& cdr) {
  return std::make_shared<ConsPair>(car, cdr);
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
    auto ap_ap = std::dynamic_pointer_cast<ApplyPair>(ap);
    std::cerr << std::string(level, '.') << "(" << std::endl;
    dump(ap_ap->lhs, level+1);
    std::cerr << std::endl;
    dump(ap_ap->rhs, level+1);
    std::cerr << std::endl;
    std::cerr << std::string(level, '.') << ")";
  } else if (ap->is_partial()) {
    auto ap_pf = std::dynamic_pointer_cast<Partial>(ap);
    std::cerr << std::string(level, '.') << "P[" << ap_pf->func_name << "]";
  } else if (ap->is_object()) {
    auto ap_obj = std::dynamic_pointer_cast<Object>(ap);
    std::cerr << std::string(level, '.');
    auto type = ap_obj->ins.type;
    switch (type) {
      case TokenType::Equality: std::cerr << "="; break;
      case TokenType::Succ: std::cerr << "Inc"; break;
      case TokenType::Pred: std::cerr << "Dec"; break;
      case TokenType::Sum: std::cerr << "+"; break;
      case TokenType::Product: std::cerr << "*"; break;
      case TokenType::Division: std::cerr << "/"; break;
      case TokenType::Eq: std::cerr << "=="; break;
      case TokenType::Lt: std::cerr << "<"; break;
      case TokenType::S: std::cerr << "S"; break;
      case TokenType::B: std::cerr << "B"; break;
      case TokenType::C: std::cerr << "C"; break;
      case TokenType::Pwr2: std::cerr << "Pwr2"; break;
      case TokenType::I: std::cerr << "I"; break;
      case TokenType::True: std::cerr << "True"; break;
      case TokenType::False: std::cerr << "False"; break;
      case TokenType::Nil: std::cerr << "Nil"; break;
      case TokenType::IsNil: std::cerr << "IsNil"; break;
      case TokenType::Cons: std::cerr << "Cons"; break;
      case TokenType::Car: std::cerr << "Car"; break;
      case TokenType::Cdr: std::cerr << "Cdr"; break;
      case TokenType::Number: std::cerr << "N[" << ap_obj->ins.immediate << "]"; break;
      case TokenType::Variable: std::cerr << "Var[" << ap_obj->ins.immediate << "]"; break;
      default: std::cerr << static_cast<int>(ap_obj->ins.type);
    }
  } else if (ap->is_cons_pair()) {
    auto ap_cons = std::dynamic_pointer_cast<ConsPair>(ap);
    std::cerr << std::string(level, '.') << "'(" << std::endl;
    dump(ap_cons->car, level+1);
    std::cerr << std::endl;
    dump(ap_cons->cdr, level+1);
    std::cerr << std::endl;
    std::cerr << std::string(level, '.') << ")";
  } else {
    throw std::runtime_error("Unknown element");
  }
}

void dump(const ApplyPtr& ap) {
  dump(ap, 0);
  std::cerr << std::endl;
}

int64_t get_int(const ApplyPtr& ap) {
  if (ap->is_object()) {
    auto ap_obj = std::dynamic_pointer_cast<Object>(ap);
    if (ap_obj->ins.type != TokenType::Number) {
      dump(ap);
      throw std::runtime_error("Failed to get_int: not a Number");
    }
    return ap_obj->ins.immediate;
  } else {
    dump(ap);
    throw std::runtime_error("Failed to get_int: not an Object");
  }
}

ApplyPtr eval(const ApplyPtr& ap, std::shared_ptr<Environment> const& env) {
  if (ap->is_apply()) {
    auto ap_ap = std::dynamic_pointer_cast<ApplyPair>(ap);
    auto first = eval(ap_ap->lhs, env);
    if (first->is_object()) {
      auto op = std::dynamic_pointer_cast<Object>(first);
      switch (op->ins.type) {
        case TokenType::Succ:
          {
            auto num = eval(ap_ap->rhs, env);
            return make_apply(number(get_int(num) + 1));
          }
        case TokenType::Pred:
          {
            auto num = eval(ap_ap->rhs, env);
            return make_apply(number(get_int(num) - 1));
          }
        case TokenType::Sum:
          {
            auto lhs = ap_ap->rhs;
            return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              auto lval = eval(lhs, env);
              auto rval = eval(rhs, env);
              return make_apply(number(get_int(lval) + get_int(rval)));
            }, "Sum1");
          }
        case TokenType::Product:
          {
            auto lhs = ap_ap->rhs;
            return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              auto lval = eval(lhs, env);
              auto rval = eval(rhs, env);
              return make_apply(number(get_int(lval) * get_int(rval)));
            }, "Product1");
          }
        case TokenType::Division:
          {
            auto lhs = ap_ap->rhs;
            return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              const auto lval = eval(lhs, env);
              const auto rval = eval(rhs, env);
              return make_apply(number(get_int(lval) / get_int(rval)));
            }, "Division1");
          }
        case TokenType::Eq:
          {
            auto lhs = ap_ap->rhs;
            return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              auto lval = eval(lhs, env);
              auto rval = eval(rhs, env);
              bool eq = false;
              if (lval->is_object() && rval->is_object()) {
                auto lobj = std::dynamic_pointer_cast<Object>(lval);
                auto robj = std::dynamic_pointer_cast<Object>(rval);
                if (lobj->ins.type == TokenType::Number && robj->ins.type == TokenType::Number) {
                  eq = lobj->ins.immediate == robj->ins.immediate;
                } else {
                  throw std::runtime_error("Unimplemented compare");
                }
              } else {
                throw std::runtime_error("Unimplemented compare");
              }
              return make_apply({eq ? TokenType::True : TokenType::False, 0});
            }, "Eq1");
          }
        case TokenType::Lt:
          {
            const auto lhs = ap_ap->rhs;
            return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              const auto lval = eval(lhs, env);
              const auto rval = eval(rhs, env);
              return make_apply({get_int(lval) < get_int(rval) ? TokenType::True : TokenType::False, 0});
            }, "Lt1");
          }
        case TokenType::Negate:
          {
            const auto val = eval(ap_ap->rhs, env);
            return make_apply(number(-get_int(val)));
          }
        case TokenType::S:
          {
            auto lhs = ap_ap->rhs;
            return make_apply([=] (const ApplyPtr& mhs) -> ApplyPtr {
              return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
                return eval(make_apply(make_apply(lhs, rhs), make_apply(mhs, rhs)), env);
              }, "S2");
            }, "S1");
          }
        case TokenType::C:
          {
            auto lhs = ap_ap->rhs;
            return make_apply([=] (const ApplyPtr& mhs) -> ApplyPtr {
              return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
                return eval(make_apply(make_apply(lhs, rhs), mhs), env);
              }, "C2");
            }, "C1");
          }
        case TokenType::B:
          {
            auto lhs = ap_ap->rhs;
            return make_apply([=] (const ApplyPtr& mhs) -> ApplyPtr {
              return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
                return eval(make_apply(lhs, eval(make_apply(mhs, rhs), env)), env);
              }, "B2");
            }, "B1");
          }
        case TokenType::True:
          {
            auto lhs = ap_ap->rhs;
            return make_apply([=] ([[maybe_unused]] const ApplyPtr& rhs) -> ApplyPtr {
              return eval(lhs, env);
            }, "True1");
          }
        case TokenType::False:
          {
            return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              return eval(rhs, env);
            }, "False1");
          }
        case TokenType::Pwr2:
          {
            auto num = eval(ap_ap->rhs, env);
            return make_apply(number(INT64_C(1) << get_int(num)));
          }
        case TokenType::I:
          return eval(ap_ap->rhs, env);
        case TokenType::Cons:
          {
            auto lhs = ap_ap->rhs;
            return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              return make_cons_pair(lhs, rhs);
            }, "Cons1");
          }
        case TokenType::Car:
          {
            auto cons_pair = std::dynamic_pointer_cast<ConsPair>(eval(ap_ap->rhs, env));
            return eval(cons_pair->car, env);
          }
        case TokenType::Cdr:
          {
            auto cons_pair = std::dynamic_pointer_cast<ConsPair>(eval(ap_ap->rhs, env));
            return eval(cons_pair->cdr, env);
          }
        case TokenType::IsNil: // ???
          {
            const auto l = eval(ap_ap->rhs, env);
            if(l->is_object()) {
              if(std::dynamic_pointer_cast<Object>(l)->ins.type == TokenType::Nil) return make_apply({TokenType::True, 1});
              else                              return make_apply({TokenType::False, 0});
            } else                              return make_apply({TokenType::False, 0});
          }
        case TokenType::Nil:
          return make_apply({TokenType::True, 1});
        default:
          throw std::runtime_error("BAD apply: ins type is " + std::to_string(static_cast<int>(op->ins.type)));
      }
    } else if (first->is_partial()) {
      return eval(std::dynamic_pointer_cast<Partial>(first)->func(ap_ap->rhs), env);
    } else if (first->is_cons_pair()) {
      auto ap_cons = std::dynamic_pointer_cast<ConsPair>(first);
      auto expr = make_apply(make_apply(ap_ap->rhs, ap_cons->car), ap_cons->cdr);
      return eval(expr, env);
    } else {
      throw std::runtime_error("BAD apply: illegal element");
    }
  } else if (ap->is_object()) {
    auto ap_obj = std::dynamic_pointer_cast<Object>(ap);
    if(ap_obj->ins.type == TokenType::Variable && env->count(ap_obj->ins.immediate)) {
      return (*env)[ap_obj->ins.immediate] = eval(env->at(ap_obj->ins.immediate), env);
    } else {
      return ap;
    }
  } else {
    return ap;
  }
}


Interpreter::Interpreter()
    : env(std::make_shared<Environment>())
{}

void Interpreter::run(const std::string& prog) {
    run(tokenize(prog));
}

void Interpreter::run(const std::vector<Token>& tokens) {
    if(tokens.empty()) return;
    if(tokens.size() >= 2u && tokens[0].type == TokenType::Variable && tokens[1].type == TokenType::Equality) { // decl
        const auto id = tokens[0].immediate;
        (*env)[id] = parse(tokens.begin() + 2, tokens.end()).first;
    } else { // eval
        auto tree = parse(tokens);
        dump(tree);
        tree = eval(tree, env);
        std::cout << tree->ins.immediate << std::endl;
    }
}
