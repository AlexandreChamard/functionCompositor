
#pragma once

#include <string>
#include <iostream>
#include <functional>

// unary       ::=     ('-' | '+') unary | primary
// primary     ::=     secondary (('+' | '-') secondary)*
// secondary   ::=     operand (('*' | '/' | '%') operand)*
// operand     ::=     '(' primary ')' | number
// number      ::=     [0-9]+

struct Res {
    bool b;
    std::string str;
    std::function<int()> f;

    Res(bool b, std::string const &str, decltype(f) const &f = nullptr): str{str}, b{b}, f{f} {}

    operator bool() const {return b;}
    bool valid() const {return b && !str.length();}
    int operator()() const { return f(); }
};

inline std::ostream &operator<<(std::ostream &os, Res const &r) {
    return os << std::boolalpha << "{" << r.b << ", \"" << r.str << "\"}";
}

struct Evalexpr {

    static inline Res pExpr(std::string const &str) {
        return pUnary(str);
    }

    static inline Res pUnary(std::string const &str) {
        if (str.empty())
            return Res{false, str};

        auto r1 = pChar<'-'>(str);
        if (r1) {
            auto r2 = pUnary(r1.str);
            if (!r2) return {false, str};
            return Res{true, r2.str,
                [f = r2.f] {
                    return -f();
            }};
        }
        r1 = pChar<'+'>(str);
        if (r1) {
            auto r2 = pUnary(r1.str);
            if (!r2) return {false, str};
            return r2; // combine with '+' does nothing more
        }
        return pPrimary(str);
    }

    static inline Res pPrimary(std::string const &str) {
        auto r1 = pSecondary(str);

        do {
            if (auto r2 = pChar<'-'>(r1.str)) {
                auto r3 = pSecondary(r2.str);
                if (!r3) return {false, str};

                r1 = {true, r3.str,
                    [a = r1.f, b = r3.f] {
                        return a() - b();
                }};
            } else if (r2 = pChar<'+'>(r1.str)) {
                auto r3 = pSecondary(r2.str);
                if (!r3) return {false, str};

                r1 = {true, r3.str,
                    [a = r1.f, b = r3.f] {
                        return a() + b();
                }};
            } else {
                break;
            }
        } while (true);
        return r1;
    }

    static inline Res pSecondary(std::string const &str) {
        auto r1 = pOperand(str);

        do {
            if (auto r2 = pChar<'*'>(r1.str)) {
                auto r3 = pOperand(r2.str);
                if (!r3) return {false, str};

                r1 = {true, r3.str,
                    [a = r1.f, b = r3.f] {
                        return a() * b();
                }};
            } else if (r2 = pChar<'/'>(r1.str)) {
                auto r3 = pOperand(r2.str);
                if (!r3) return {false, str};

                r1 = {true, r3.str,
                    [a = r1.f, b = r3.f] {
                        return a() / b();
                }};
            } else if (r2 = pChar<'%'>(r1.str)) {
                auto r3 = pOperand(r2.str);
                if (!r3) return {false, str};

                r1 = {true, r3.str,
                    [a = r1.f, b = r3.f] {
                        return static_cast<int>(a()) % static_cast<int>(b());
                }};
            } else {
                break;
            }
        } while (true);

        return r1;
    }

    static inline Res pOperand(std::string const &str) {
        auto r1 = pChar<'('>(str);
        if (!r1) return pNumber(str);

        auto r2 = pUnary(r1.str);
        if (!r2) return {false, str};

        auto r3 = pChar<')'>(r2.str);
        if (!r3) return {false, str};

        return {true, r3.str, r2.f};
    }

    static inline Res pNumber(std::string const &str) {
        std::size_t i = 0;
        for (; i < str.length() && str[i] >= '0' && str[i] <= '9'; ++i);
        if (i == 0) return {false, str};

        return {true, str.substr(i),
            [n = std::atoi(str.substr(0, i).c_str())] {
                return n;
        }};
    }

    template<char c>
    static inline Res pChar(std::string const &str) {
        if (str.empty() || str.front() != c)
            return {false, str};
        return {true, std::string{str.data()+1}};
    }
};
