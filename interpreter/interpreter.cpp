#include <iostream>
#include <stdexcept>
#include "lexer.hpp"
#include "interpreter.hpp"

auto id = [] (const ApplyPtr& ap) { return ap; };

Object::Object(const Token& ins) : Apply(), ins(ins) {}
Partial::Partial(std::function<ApplyPtr(const ApplyPtr&)> func, const std::string& func_name)
    : Apply(), func(func), func_name(func_name) {}
ApplyPair::ApplyPair(const ApplyPtr& lhs, const ApplyPtr& rhs)
    : Apply(), lhs(lhs), rhs(rhs) {}
ConsPair::ConsPair(const ApplyPtr& car, const ApplyPtr& cdr)
    : Apply(), car(car), cdr(cdr) {}

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

bool isnil(const ApplyPtr& ap) {
  if(ap->evaluated) return isnil(ap->evaluated);
  if(ap->is_object()) {
    return std::dynamic_pointer_cast<Object>(ap)->ins.type == TokenType::Nil;
  } else {
    return false;
  }
}

std::string string_of_tree(const ApplyPtr& ap, int level, bool enable_eval, const std::shared_ptr<Environment>& env) {
  if(ap->evaluated) return string_of_tree(ap->evaluated, level, enable_eval, env);
  std::string res;
  if (ap->is_apply()) {
    if (enable_eval) {
      res += string_of_tree(eval(ap, env), level, enable_eval, env);
    } else {
      auto ap_ap = std::dynamic_pointer_cast<ApplyPair>(ap);
      res += "(";
      res += string_of_tree(ap_ap->lhs, level+1, enable_eval, env);
      res += " ";
      res += string_of_tree(ap_ap->rhs, level+1, enable_eval, env);
      res += ")";
    }
  } else if (ap->is_partial()) {
    auto ap_pf = std::dynamic_pointer_cast<Partial>(ap);
    res += "P[" + ap_pf->func_name + "]";
  } else if (ap->is_object()) {
    auto ap_obj = std::dynamic_pointer_cast<Object>(ap);
    auto type = ap_obj->ins.type;
    switch (type) {
      case TokenType::Equality: res += "="; break;
      case TokenType::Succ: res += "Inc"; break;
      case TokenType::Pred: res += "Dec"; break;
      case TokenType::Sum: res += "+"; break;
      case TokenType::Product: res += "*"; break;
      case TokenType::Division: res += "/"; break;
      case TokenType::Eq: res += "=="; break;
      case TokenType::Lt: res += "<"; break;
      case TokenType::S: res += "S"; break;
      case TokenType::B: res += "B"; break;
      case TokenType::C: res += "C"; break;
      case TokenType::Pwr2: res += "Pwr2"; break;
      case TokenType::I: res += "I"; break;
      case TokenType::True: res += "True"; break;
      case TokenType::False: res += "False"; break;
      case TokenType::Nil: res += "Nil"; break;
      case TokenType::IsNil: res += "IsNil"; break;
      case TokenType::Cons: res += "Cons"; break;
      case TokenType::Car: res += "Car"; break;
      case TokenType::Cdr: res += "Cdr"; break;
      case TokenType::Number: res += std::to_string(ap_obj->ins.immediate); break;
      case TokenType::Variable:
        if (enable_eval) {
          res += string_of_tree(eval(env->at(ap_obj->ins.immediate), env), level, enable_eval, env);
        } else {
          res += "Var[" + std::to_string(ap_obj->ins.immediate) + "]"; break;
        }
        break;
      default: res += std::to_string(static_cast<int>(ap_obj->ins.type));
    }
  } else if (ap->is_cons_pair()) {
    auto ap_cons = std::dynamic_pointer_cast<ConsPair>(ap);
    res += "[";
    res += string_of_tree(ap_cons->car, level+1, enable_eval, env);
    res += ", ";
    while (ap_cons->cdr->is_cons_pair()) {
      if(ap_cons->cdr->evaluated) ap_cons = std::dynamic_pointer_cast<ConsPair>(ap_cons->cdr->evaluated);
      else                        ap_cons = std::dynamic_pointer_cast<ConsPair>(ap_cons->cdr);
      res += string_of_tree(ap_cons->car, level+1, enable_eval, env);
      res += ", ";
    }
    res += string_of_tree(ap_cons->cdr, level+1, enable_eval, env);
    res += "]";
  } else {
    throw std::runtime_error("Unknown element");
  }
  return res;
}

std::string string_of_tree(const ApplyPtr& ap, bool enable_eval, const std::shared_ptr<Environment>& env) {
  return string_of_tree(ap, 0, enable_eval, env) + "\n";
}

int64_t get_int(const ApplyPtr& ap) {
  if(ap->evaluated) return get_int(ap->evaluated);
  if (ap->is_object()) {
    auto ap_obj = std::dynamic_pointer_cast<Object>(ap);
    if (ap_obj->ins.type != TokenType::Number) {
      throw std::runtime_error("Failed to get_int: not a Number");
    }
    return ap_obj->ins.immediate;
  } else {
    throw std::runtime_error("Failed to get_int: not an Object");
  }
}

ApplyPtr eval(const ApplyPtr& ap, std::shared_ptr<Environment> const& env) {
  if(ap->evaluated) return ap->evaluated;
  if (ap->is_apply()) {
    const auto ap_ap = [&] () {
      if(ap->evaluated) return std::dynamic_pointer_cast<ApplyPair>(ap->evaluated);
      else              return std::dynamic_pointer_cast<ApplyPair>(ap);
    }();
    auto first = eval(ap_ap->lhs, env);
    if (first->is_object()) {
      auto op = std::dynamic_pointer_cast<Object>(first);
      switch (op->ins.type) {
        case TokenType::Succ:
          {
            auto num = eval(ap_ap->rhs, env);
            return ap->evaluated = make_apply(number(get_int(num) + 1));
          }
        case TokenType::Pred:
          {
            auto num = eval(ap_ap->rhs, env);
            return ap->evaluated = make_apply(number(get_int(num) - 1));
          }
        case TokenType::Sum:
          {
            auto lhs = ap_ap->rhs;
            return ap->evaluated = make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              auto lval = eval(lhs, env);
              auto rval = eval(rhs, env);
              return make_apply(number(get_int(lval) + get_int(rval)));
            }, "Sum1");
          }
        case TokenType::Product:
          {
            auto lhs = ap_ap->rhs;
            return ap->evaluated = make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              auto lval = eval(lhs, env);
              auto rval = eval(rhs, env);
              return make_apply(number(get_int(lval) * get_int(rval)));
            }, "Product1");
          }
        case TokenType::Division:
          {
            auto lhs = ap_ap->rhs;
            return ap->evaluated = make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              const auto lval = eval(lhs, env);
              const auto rval = eval(rhs, env);
              return make_apply(number(get_int(lval) / get_int(rval)));
            }, "Division1");
          }
        case TokenType::Eq:
          {
            auto lhs = ap_ap->rhs;
            return ap->evaluated = make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
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
            return ap->evaluated = make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              const auto lval = eval(lhs, env);
              const auto rval = eval(rhs, env);
              return make_apply({get_int(lval) < get_int(rval) ? TokenType::True : TokenType::False, 0});
            }, "Lt1");
          }
        case TokenType::Negate:
          {
            const auto val = eval(ap_ap->rhs, env);
            return ap->evaluated = make_apply(number(-get_int(val)));
          }
        case TokenType::S:
          {
            auto lhs = ap_ap->rhs;
            return ap->evaluated = make_apply([=] (const ApplyPtr& mhs) -> ApplyPtr {
              return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
                return eval(make_apply(make_apply(lhs, rhs), make_apply(mhs, rhs)), env);
              }, "S2");
            }, "S1");
          }
        case TokenType::C:
          {
            auto lhs = ap_ap->rhs;
            return ap->evaluated = make_apply([=] (const ApplyPtr& mhs) -> ApplyPtr {
              return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
                return eval(make_apply(make_apply(lhs, rhs), mhs), env);
              }, "C2");
            }, "C1");
          }
        case TokenType::B:
          {
            auto lhs = ap_ap->rhs;
            return ap->evaluated = make_apply([=] (const ApplyPtr& mhs) -> ApplyPtr {
              return make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
                return eval(make_apply(lhs, eval(make_apply(mhs, rhs), env)), env);
              }, "B2");
            }, "B1");
          }
        case TokenType::True:
          {
            auto lhs = ap_ap->rhs;
            return ap->evaluated = make_apply([=] ([[maybe_unused]] const ApplyPtr& rhs) -> ApplyPtr {
              return eval(lhs, env);
            }, "True1");
          }
        case TokenType::False:
          {
            return ap->evaluated = make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              return eval(rhs, env);
            }, "False1");
          }
        case TokenType::Pwr2:
          {
            auto num = eval(ap_ap->rhs, env);
            return ap->evaluated = make_apply(number(INT64_C(1) << get_int(num)));
          }
        case TokenType::I:
          return ap->evaluated = eval(ap_ap->rhs, env);
        case TokenType::Cons:
          {
            auto lhs = ap_ap->rhs;
            return ap->evaluated = make_apply([=] (const ApplyPtr& rhs) -> ApplyPtr {
              return make_cons_pair(lhs, rhs);
            }, "Cons1");
          }
        case TokenType::Car:
          {
            const auto val = eval(ap_ap->rhs, env);
            if(!val->is_cons_pair()) throw std::runtime_error("BAD apply: expect cons pair");
            auto cons_pair = std::dynamic_pointer_cast<ConsPair>(val);
            return ap->evaluated = eval(cons_pair->car, env);
          }
        case TokenType::Cdr:
          {
            const auto val = eval(ap_ap->rhs, env);
            if(!val->is_cons_pair()) throw std::runtime_error("BAD apply: expect cons pair");
            auto cons_pair = std::dynamic_pointer_cast<ConsPair>(val);
            return ap->evaluated = eval(cons_pair->cdr, env);
          }
        case TokenType::IsNil: // ???
          {
            const auto l = eval(ap_ap->rhs, env);
            if(isnil(l)) {
              return ap->evaluated = make_apply({TokenType::True, 1});
            } else {
              return ap->evaluated = make_apply({TokenType::False, 0});
            }
          }
        case TokenType::Nil:
          return ap->evaluated = make_apply({TokenType::True, 1});
        default:
          throw std::runtime_error("BAD apply: ins type is " + std::to_string(static_cast<int>(op->ins.type)));
      }
    } else if (first->is_partial()) {
      return ap->evaluated = eval(std::dynamic_pointer_cast<Partial>(first)->func(ap_ap->rhs), env);
    } else if (first->is_cons_pair()) {
      auto ap_cons = std::dynamic_pointer_cast<ConsPair>(first);
      auto expr = make_apply(make_apply(ap_ap->rhs, ap_cons->car), ap_cons->cdr);
      return ap->evaluated = eval(expr, env);
    } else {
      throw std::runtime_error("BAD apply: illegal element");
    }
  } else if (ap->is_object()) {
    const auto ap_obj = [&] () {
      if(ap->evaluated) return std::dynamic_pointer_cast<Object>(ap->evaluated);
      else  return std::dynamic_pointer_cast<Object>(ap);
    }();
    const auto imm = ap_obj->ins.immediate;
    if(ap_obj->ins.type == TokenType::Variable && env->count(imm)) {
      return eval(env->at(imm), env);
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

void Interpreter::run(std::ostream& os, const std::string& prog) {
  run(os, tokenize(prog));
}
void Interpreter::run(const std::string& prog) {
  run(std::cout, tokenize(prog));
}

void Interpreter::run(std::ostream& os, const std::vector<Token>& tokens) {
  if(tokens.empty()) return;
  if(tokens.size() >= 2u && tokens[0].type == TokenType::Variable && tokens[1].type == TokenType::Equality) { // decl
    const auto id = tokens[0].immediate;
    (*env)[id] = parse(tokens.begin() + 2, tokens.end()).first;
  } else { // eval
    auto tree = parse(tokens);
    tree = eval(tree, env);
    os << string_of_tree(tree, true, env);
  }
}
void Interpreter::run(const std::vector<Token>& tokens) {
  run(std::cout, tokens);
}
