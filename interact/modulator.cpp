#include <algorithm>
#include <iostream>
#include <string>

using namespace std;

string string_of_ll(long long x) {
    string s = "";
    if (x >= 0)
        s += "01";
    else
        s += "10";
    x = abs(x);
    long long tmp = 1, lg = 0;
    while (tmp <= x) {
        s.push_back('1');
        tmp <<= 4;
        lg += 4;
    }
    s.push_back('0');
    string digit = "";
    for (long long i = 0; i < lg; i++) {
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
    for (; idx < (int)s.size(); idx++) {
        if (s[idx] == '[') {
            idx++;
            string ret = parse_list(s, idx);
            if(s[idx] == ']'){
                idx--;
            }else{
                ans += "11";
            }
            ans += ret;
        } else if (s[idx] == ']') {
            idx++;
            break;
        } else if (isdigit(s[idx]) || s[idx] == '-') {
            bool minus = (s[idx] == '-');
            if (minus) idx++;
            long long num = 0;
            for (; idx < (int)s.size(); idx++) {
                if (isdigit(s[idx])) {
                    num = num * 10 + (s[idx] - '0');
                } else {
                    idx--;
                    break;
                }
            }
            if (minus) num = -num;
            if(s[idx + 1] != ']') ans += "11";
            ans += string_of_ll(num);
        } else if (s[idx] == 'N'){
            if(s[idx + 3] != ']') ans += "11";
            ans += "00";
        }
    }
    return ans;
}

string modulate(const string s) {
    if (isdigit(s[0]) || s[0] == '-') {
        return string_of_ll(stoi(s));
    } else {
        string t;
        for(auto &c : s) if(c != ' ') t.push_back(c);
        int idx = 1;
        return parse_list(t, idx);
    }
}