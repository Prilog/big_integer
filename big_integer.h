#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstdio>
#include <vector>
#include <string>
#include "neko_vector.h"

using std::vector;
using std::string;

struct big_integer {
    typedef unsigned int ui;
    typedef unsigned long long ull;
public:
    big_integer();
    big_integer(int arg);
    big_integer(unsigned int arg);
    big_integer(ull arg);
    big_integer(const big_integer& arg);
    explicit big_integer(const string& arg);
    ~big_integer();

    void clear();

    string get_binary_string() const;
    string get_decimal_string() const;
    bool signum;

    big_integer& operator= (const big_integer& arg);

    big_integer& operator+= (const big_integer& arg);
    big_integer& operator-= (const big_integer& arg);

    big_integer operator- () const;
    big_integer operator+ () const;
    big_integer& operator*= (const big_integer& arg);

    big_integer operator~ () const;
    big_integer& operator&= (const big_integer& arg);
    big_integer& operator|= (const big_integer& arg);
    big_integer& operator^= (const big_integer& arg);

    big_integer& operator/= (const big_integer& arg);
    big_integer& operator%= (const big_integer& arg);

    big_integer& operator<<= (int arg);
    big_integer& operator>>= (int arg);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

private:
    const ull base = 4294967296;
    const ui max_ui = 4294967295;
    const int min_int = -2147483648;
    const int max_int = 2147483647;
    neko_vector digits;

    ui operator[] (size_t ind) const;
    ui& operator[] (size_t ind);

    void delete_space();

    bool is_zero() const;

    void make_bincode();

    big_integer sub_big_integer(size_t from, size_t to);

    ui small_unsigned_mod(const big_integer& a, ui b);
    void small_unsigned_sum(big_integer& a, ui b, size_t pos);
    void small_unsigned_multiply(big_integer& a, ui b);
    void small_unsigned_div(big_integer& a, ui b);
    void small_unsigned_sub(big_integer& a, ui b, size_t pos);

    void big_unsigned_add(const big_integer& a);
    void big_unsigned_sub(const big_integer& a);
    void big_unsigned_sub(const big_integer& a, size_t from);
    void big_unsigned_mul(const big_integer& a);
    std::pair<big_integer, big_integer> big_unsigned_div(const big_integer& a, const big_integer& arg);

    ui big_unsigned_cmp(const big_integer& a) const;

    void invert_bits();
};

big_integer operator+ (big_integer a, const big_integer& b);
big_integer operator- (big_integer a, const big_integer& b);
big_integer operator* (big_integer a, const big_integer& b);

big_integer operator/ (big_integer a, const big_integer& b);
big_integer operator% (big_integer a, const big_integer& b);

big_integer operator& (big_integer a, const big_integer& b);
big_integer operator| (big_integer a, const big_integer& b);
big_integer operator^ (big_integer a, const big_integer& b);

big_integer operator<< (big_integer a, int b);
big_integer operator>> (big_integer a, int b);

string to_string(const big_integer& arg);
std::ostream& operator<< (std::ostream& os, const big_integer& data);

#endif // BIG_INTEGER_H
