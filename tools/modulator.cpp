#include <algorithm>
#include <iostream>
#include <string>

using namespace std;

string string_of_int(int x) {
  string s = "";
  if (x >= 0)
    s += "01";
  else
    s += "10";
  x = abs(x);
  int tmp = 1, lg = 0;
  while (tmp <= x) {
    s.push_back('1');
    tmp <<= 4;
    lg += 4;
  }
  s.push_back('0');
  string digit = "";
  for (int i = 0; i < lg; i++) {
    digit.push_back(x & 1 ? '1' : '0');
    x >>= 1;
  }
  reverse(digit.begin(), digit.end());
  s += digit;
  return s;
}

string parse_list(const string &s, int &idx) {
  string ans;
  string tmp;
  for (; idx < s.size(); idx++) {
    if (s[idx] == '[') {
      idx++;
      ans += parse_list(s, idx);
    } else if (s[idx] == ']') {
      ans += "00";
      break;
    } else if (isdigit(s[idx]) || s[idx] == '-') {
      ans += "11";
      bool minus = (s[idx] == '-');
      if (minus) idx++;
      int num = 0;
      for (; idx < s.size(); idx++) {
        if (isdigit(s[idx])) {
          num = num * 10 + (s[idx] - '0');
        } else {
            idx--;
          break;
        }
      }
      if (minus) num = -num;
      ans += string_of_int(num);
    } else if (s[idx] == 'N'){
        ans += "1100";
    }
  }
  return ans;
}

string modulate(const string s) {
  if (isdigit(s[0]) || s[0] == '-') {
    return string_of_int(stoi(s));
  } else {
    int idx = 0;
    return parse_list(s, idx);
  }
}