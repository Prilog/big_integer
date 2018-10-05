#include "big_integer.h"

#include <cstdio>
#include <vector>
#include <string>
#include <algorithm>

typedef unsigned int ui;
typedef unsigned long long ull;

using std::vector;
using std::string;

const ull base = 4294967296;
const ui max_ui = 4294967295;

big_integer::big_integer() {
	signum = 0;
	digits.clear();
	digits.push_back(0);
}

big_integer::~big_integer() {
	digits.clear();
}

big_integer::big_integer(int arg) {
	digits.clear();
	if (arg != min_int) {
		digits.push_back(abs(arg));
	}
	else {
		digits.push_back(ui(max_int) + 1);
	}
	if (arg >= 0) {
		signum = 0;
	}
	else {
		signum = 1;
	}
}

big_integer::big_integer(unsigned int arg) {
	signum = 0;
	digits.clear();
	digits.push_back(arg);
}

big_integer::big_integer(ull arg) {
	signum = 0;
	digits.clear();
	digits.push_back(ui(arg % base));
	digits.push_back(ui(arg / base));
	delete_space();
}

big_integer::big_integer(const big_integer& arg) {
	signum = arg.signum;
	digits.clear();
	digits = arg.digits;
}

big_integer::big_integer(const string& arg) {
	size_t i = 0;
	signum = 0;
	if (arg[0] == '-') {
		i = 1;
		signum = 1;
	}
	digits.clear();
	digits.push_back(0);
	for (; i < arg.size(); i++) {
		small_unsigned_multiply(*this, 10);
		small_unsigned_sum(*this, arg[i] - '0', 0);
	}
}

void big_integer::clear() {
	signum = 0;
	digits.clear();
	digits.push_back(0);
}

string big_integer::get_binary_string() const {
	if (is_zero()) {
		return "0b0";
	}
	big_integer buf = *this;
	string res = "";
	buf.make_bincode();
	ui soft = 1;
	if (!signum) {
		res += "0b";
	}
	else {
		res += "1b1";
		soft = 0;
	}
	bool zero_flag = true;
	for (size_t i = digits.size() - 1; ; i--) {
		for (size_t j = 0; j < 32; j++) {
			ui num = (buf.digits[i] << j) >> 31;
			if (zero_flag && num == soft) {
				zero_flag = false;
				res += soft + '0';
			}
			else if (!zero_flag) {
				res += num + '0';
			}
		}
		if (i == 0) {
			break;
		}
	}
	return res;
}

string big_integer::get_decimal_string() const {
	if (is_zero()) {
		return "0";
	}
	string res = "";
	big_integer buf = *this;
	while (!buf.is_zero()) {
		res += buf.small_unsigned_mod(buf, 10) + '0';
		buf.small_unsigned_div(buf, 10);
	}
	if (signum) {
		res += '-';
	}
	std::reverse(res.begin(), res.end());
	return res;
}

big_integer& big_integer::operator= (const big_integer& arg) {
	signum = arg.signum;
	digits = arg.digits;
	return *this;
}
big_integer operator+ (big_integer a, const big_integer& b) {
	return a += b;
}
big_integer& big_integer::operator+= (const big_integer& arg) {
	if (signum == arg.signum) {
		big_unsigned_add(arg);
		return *this;
	}
	ui flag = big_unsigned_cmp(arg);
	if (flag == 0) {
		clear();
		return *this;
	}
	if (flag == 2) {
		big_integer res = arg;
		res.big_unsigned_sub(*this);
		*this = res;
		return *this;
	}
	else {
		big_unsigned_sub(arg);
		return *this;
	}
}
big_integer operator- (big_integer a, const big_integer& b) {
	return a -= b;
}
big_integer& big_integer::operator-= (const big_integer& arg) {
	if (signum != arg.signum) {
		big_unsigned_add(arg);
		return *this;
	}
	ui flag = big_unsigned_cmp(arg);
	if (flag == 0) {
		clear();
		return *this;
	}
	if (flag == 2) {
		big_integer res = arg;
		res.big_unsigned_sub(*this);
		res.signum = !res.signum;
		*this = res;
		return *this;
	}
	else {
		big_unsigned_sub(arg);
		return *this;
	}
}
big_integer big_integer::operator- () const {
	big_integer res = *this;
	res.signum = !res.signum;
	return res;
}
big_integer big_integer::operator+ () const {
	return *this;
}
big_integer operator* (big_integer a, const big_integer& b) {
	return a *= b;
}
big_integer& big_integer::operator*= (const big_integer& arg) {
	if (signum == arg.signum) {
		signum = 0;
	}
	else {
		signum = 1;
	}
	big_unsigned_mul(arg);
	return *this;
}
big_integer big_integer::operator~ () const {
	if (is_zero()) {
		return big_integer(-1);
	}
	big_integer res = *this;
	if (!signum) {
		res = -(res + 1);
	}
	else {
		res.signum = 0;
		res.invert_bits();
		res += 1;
		res.invert_bits();
	}
	res.delete_space();
	return res;
}
big_integer operator& (big_integer a, const big_integer& b) {
	return a &= b;
}
big_integer& big_integer::operator&= (const big_integer& arg) {
	big_integer a = *this;
	a.make_bincode();
	big_integer b = arg;
	b.make_bincode();
	if (a.digits.size() < b.digits.size()) {
		std::swap(a, b);
	}
	for (size_t i = 0; i < a.digits.size(); i++) {
		if (i < b.digits.size()) {
			a.digits[i] &= b.digits[i];
		}
		else {
			if (!b.signum) {
				a.digits[i] = 0;
			}
		}
	}
	a.signum = 0;
	a.delete_space();
	*this = a;
	return *this;
}
big_integer operator| (big_integer a, const big_integer& b) {
	return a |= b;
}
big_integer& big_integer::operator|= (const big_integer& arg) {
	big_integer a = *this;
	a.make_bincode();
	big_integer b = arg;
	b.make_bincode();
	if (a.digits.size() < b.digits.size()) {
		std::swap(a, b);
	}
	for (size_t i = 0; i < a.digits.size(); i++) {
		if (i < b.digits.size()) {
			a.digits[i] |= b.digits[i];
		}
		else {
			if (b.signum) {
				a.digits[i] = 1;
			}
		}
	}
	a.signum = (a.signum | b.signum);
	if (a.signum) {
		a.invert_bits();
		a.small_unsigned_sum(a, 1, 0);
	}
	a.delete_space();
	*this = a;
	return *this;
}
big_integer operator^ (big_integer a, const big_integer& b) {
	return a ^= b;
}
big_integer& big_integer::operator^= (const big_integer& arg) {
	big_integer a = *this;
	a.make_bincode();
	big_integer b = arg;
	b.make_bincode();
	if (a.digits.size() < b.digits.size()) {
		std::swap(a, b);
	}
	for (size_t i = 0; i < a.digits.size(); i++) {
		if (i < b.digits.size()) {
			a.digits[i] ^= b.digits[i];
		}
		else {
			if (b.signum) {
				a.digits[i] ^= 0;
			}
			else {
				a.digits[i] ^= max_ui;
			}
		}
	}
	a.signum = (a.signum ^ b.signum);
	if (a.signum) {
		a.invert_bits();
		a.small_unsigned_sum(a, 1, 0);
	}
	a.delete_space();
	*this = a;
	return *this;
}
big_integer& big_integer::operator<<= (int arg) {
	for (int i = 0; i < arg; i = i + 1) {
		*this *= 2;
	}
	return *this;
}

big_integer operator<< (big_integer a, int b) {
	return a <<= b;
}

big_integer& big_integer::operator>>= (int arg) {
	make_bincode();
	for (size_t i = 0; i < digits.size(); i++) {
		for (ui j = 0; j < 32; j++) {
			ui ind = j;
			ui block = i;
			ui new_ind = (j + arg) % 32;
			ui new_block = i + ((j + arg) / 32);
			ui cur = signum;
			if (new_block < digits.size()) {
				cur = (digits[new_block] << (31 - new_ind));
				cur = cur >> 31;
			}
			if (cur == 1) {
				digits[block] |= (1 << ind);
			}
			else {
				digits[block] &= max_ui ^ (ui(1) << ind);
			}
		}
	}
	if (signum) {
		invert_bits();
		big_unsigned_add(1);
	}
	delete_space();
	return *this;
}

big_integer operator>> (big_integer a, int b) {
	return a >>= b;
}

big_integer operator/ (big_integer a, const big_integer& b) {
	return a /= b;
}

big_integer& big_integer::operator/= (const big_integer& arg) {
	big_integer counter = 0;
	std::pair<big_integer, big_integer> res = counter.big_unsigned_div(*this, arg);
	if (signum == arg.signum) {
		res.first.signum = 0;
	}
	else {
		res.first.signum = 1;
	}
	*this = res.first;
	return *this;
}

big_integer operator% (big_integer a, const big_integer& b) {
	return a %= b;
}

big_integer& big_integer::operator%= (const big_integer& arg) {
	big_integer counter = 0;
	std::pair<big_integer, big_integer> res = counter.big_unsigned_div(*this, arg);
	res.second.signum = signum;
	*this = res.second;
	return *this;
}

bool operator< (big_integer const& x, big_integer const& y) {
	bool a = x.is_zero(), b = y.is_zero();
	if (a && b) {
		return false;
	}
	if (a) {
		return !y.signum;
	}
	if (b || x.signum != y.signum) {
		return x.signum;
	}
	return x.big_unsigned_cmp(y) == 2;
}
bool operator> (big_integer const& x, big_integer const& y) {
	bool a = x.is_zero(), b = y.is_zero();
	if (a && b) {
		return false;
	}
	if (a) {
		return y.signum;
	}
	if (b || x.signum != y.signum) {
		return !x.signum;
	}
	return x.big_unsigned_cmp(y) == 1;
}
bool operator<= (big_integer const& x, big_integer const& y) {
	bool a = x.is_zero(), b = y.is_zero();
	if (a && b) {
		return false;
	}
	if (a) {
		return !y.signum;
	}
	if (b || x.signum != y.signum) {
		return x.signum;
	}
	return x.big_unsigned_cmp(y) != 1;
}
bool operator>= (big_integer const& x, big_integer const& y) {
	bool a = x.is_zero(), b = y.is_zero();
	if (a && b) {
		return true;
	}
	if (a) {
		return y.signum;
	}
	if (b || x.signum != y.signum) {
		return !x.signum;
	}
	return x.big_unsigned_cmp(y) != 2;
}

bool operator== (big_integer const& x, big_integer const& y) {
	return x.big_unsigned_cmp(y) == 0;
}

bool operator!= (big_integer const& x, big_integer const& y) {
	return x.big_unsigned_cmp(y) != 0;
}

ui big_integer::operator[] (size_t ind) const {
	return digits[ind];
}

ui& big_integer::operator[] (size_t ind) {
	return digits[ind];
}

void big_integer::delete_space() {
	while (digits.size() > 1 && digits[digits.size() - 1] == 0) {
		digits.pop_back();
	}
}

bool big_integer::is_zero() const {
	return digits.size() == 1 && digits[0] == 0;
}

void big_integer::make_bincode() {
	if (!signum) {
		return;
	}
	signum = 0;
	invert_bits();
	*this += 1;
	signum = 1;
}

big_integer big_integer::sub_big_integer(size_t from, size_t to) {
	big_integer res = 0;
	res[0] = digits[from];
	for (size_t i = from + 1; i < to + 1; i++) {
		res.digits.push_back(0);
		res[i - from] = digits[i];
	}
	return res;
}

ui big_integer::small_unsigned_mod(const big_integer& a, ui b) {
	ull res = 0;
	ull power = 1;
	ull mod = b;
	for (size_t i = 0; i < a.digits.size(); i++) {
		res = (res + (ull(a[i]) % mod) * power) % mod;
		power = (power << 32) % mod;
	}
	return ui(res);
}

void big_integer::small_unsigned_sum(big_integer& a, ui b, size_t pos) {
	if (pos == a.digits.size()) {
		a.digits.push_back(0);
	}
	ull cur = ull(a[pos]) + ull(b);
	for (size_t i = pos; i < a.digits.size(); i++) {
		if (cur >= base) {
			a[i] = cur << 32 >> 32;
			if (i + 1 == a.digits.size()) {
				a.digits.push_back(0);
				cur = cur >> 32;
			}
			else {
				cur = ull(a[i + 1]) + (cur >> 32);
			}
		}
		else {
			a[i] = cur;
			break;
		}
	}
}

void big_integer::small_unsigned_multiply(big_integer& a, ui b) {
	big_integer res(0);
	res.signum = a.signum;
	for (size_t i = 0; i < a.digits.size(); i++) {
		ull cur = a[i] * ull(b);
		if (cur >= base) {
			small_unsigned_sum(res, cur << 32 >> 32, i);
			small_unsigned_sum(res, ui(cur / base), i + 1);
		}
		else {
			small_unsigned_sum(res, cur, i);
		}
	}
	res.delete_space();
	a = res;
}

void big_integer::small_unsigned_div(big_integer& a, ui b) {
	if (a.digits.size() == 1) {
		a[0] = a[0] / b;
		return;
	}
	big_integer res(0);
	res.signum = a.signum;
	for (size_t i = 1; i < a.digits.size(); i++) {
		res.digits.push_back(0);
	}
	ull cur = a[a.digits.size() - 1];
	for (size_t i = a.digits.size() - 2; ; i--) {
		cur = (cur << 32) + a[i];
		ull buf = cur / b;
		small_unsigned_sum(res, ui(buf >> 32), i + 1);
		small_unsigned_sum(res, ui(buf << 32 >> 32), i);
		cur = cur % b;
		if (i == 0) {
			break;
		}
	}
	res.delete_space();
	a = res;
}

void big_integer::small_unsigned_sub(big_integer& a, ui b, size_t pos) {
	for (size_t i = pos; i < a.digits.size(); i++) {
		if (a[i] >= b) {
			a[i] -= b;
			break;
		}
		else {
			a[i] -= b;
			b = 1;
		}
	}
	a.delete_space();
}

void big_integer::big_unsigned_add(const big_integer& a) {
	ull buf = 0;
	for (size_t i = 0; i < a.digits.size(); i++) {
		//small_unsigned_sum(*this, a[i], i);
		if ((*this).digits.size() == i) {
			(*this).digits.push_back(0);
		}
		ull sum = ull((*this)[i]) + ull(a[i]) + buf;
		if (sum > ull(max_ui)) {
			buf = 1;
		}
		else {
			buf = 0;
		}
		(*this)[i] = ui(sum % base);
	}
	if (buf == 1) {
		if ((*this).digits.size() == a.digits.size()) {
			(*this).digits.push_back(1);
		}
		else {
			small_unsigned_sum(*this, 1, a.digits.size());
		}
	}
}

void big_integer::big_unsigned_sub(const big_integer& a) {
	ull buf = 0;
	for (size_t i = 0; i < a.digits.size(); i++) {
		//small_unsigned_sub(*this, a[i], i);
		ull red = ull((*this)[i]);
		ull sub = ull(a[i]) + buf;
		if (sub > red) {
			buf = 1;
			red += base;
		}
		else {
			buf = 0;
		}
		(*this)[i] = red - sub;
	}
	if (buf == 1) {
		small_unsigned_sub(*this, 1, a.digits.size());
	}
}

void big_integer::big_unsigned_sub(const big_integer& a, size_t from) {
	ull buf = 0;
	for (size_t i = 0; i < a.digits.size(); i++) {
		//small_unsigned_sub(*this, a[i], i);
		ull red = ull((*this)[i + from]);
		ull sub = ull(a[i]) + buf;
		if (sub > red) {
			buf = 1;
			red += base;
		}
		else {
			buf = 0;
		}
		(*this)[i + from] = red - sub;
	}
	if (buf == 1) {
		small_unsigned_sub(*this, 1, a.digits.size() + from);
	}
}

void big_integer::big_unsigned_mul(const big_integer& a) {
	big_integer res;
	res.signum = signum;
	for (size_t i = 0; i < digits.size(); i++) {
		for (size_t j = 0; j < a.digits.size(); j++) {
			ull x = ull(digits[i]) * ull(a[j]);
			small_unsigned_sum(res, (x << 32 >> 32), i + j);
			small_unsigned_sum(res, (x >> 32), i + j + 1);
		}
	}
	res.delete_space();
	*this = res;
}
std::pair<big_integer, big_integer> big_integer::big_unsigned_div(const big_integer& a, const big_integer& arg) {
	if (arg.digits.size() <= 1) {
		big_integer x = a;
		ui m = small_unsigned_mod(x, arg[0]);
		small_unsigned_div(x, arg[0]);
		return std::make_pair(x, big_integer(m));
	}
	ui cmp_res = a.big_unsigned_cmp(arg);
	if (cmp_res == 0) {
		return std::make_pair(big_integer(1), big_integer(0));
	}
	if (cmp_res == 2) {
		return std::make_pair(big_integer(0), a);
	}
	size_t n = arg.digits.size();
	big_integer A = a;
	big_integer B = arg;
	big_integer res = 0;
	ui scale = ui(base / (ull(B[n - 1]) + 1));
	if (scale > 1) {
		small_unsigned_multiply(A, scale);
		small_unsigned_multiply(B, scale);
	}
	ui m = A.digits.size() - B.digits.size();
	for (size_t i = 0; i < m; i++) {
		res.digits.push_back(0);
	}
	for (size_t i = m; ; i--) {
		size_t j = n + i;
		ull q = 0;
		if (i == m) {
			j = n + m - 1;
			if (A.digits.size() > j) {
				q = ull(A[j]) / ull(B[n - 1]);
			}
		}
		else {
			if (A.digits.size() > j) {
				q = (ull(A[j]) * base + ull(A[j - 1])) / ull(B[n - 1]);
			}
		}
		if (A.digits.size() > j) {
			if (q > max_ui) {
				q = max_ui;
			}
			big_integer sub_int = A.sub_big_integer(i, j);
			big_integer mult = B;
			small_unsigned_multiply(mult, ui(q));
			while (mult.big_unsigned_cmp(sub_int) == 1) {
				mult -= B;
				q--;
			}
			small_unsigned_sum(res, q, i);
			A.big_unsigned_sub(mult, i);
		}
		j--;
		if (i == 0) {
			break;
		}
	}
	if (A.big_unsigned_cmp(B) != 2) {
		ui q = A[n - 1] / B[n - 1];
		big_integer mult = B;
		small_unsigned_multiply(mult, q);
		while (mult > A) {
			mult -= B;
			q--;
		}
		small_unsigned_sum(res, q, 0);
		A.big_unsigned_sub(mult, 0);
	}
	A.small_unsigned_div(A, scale);
	res.delete_space();
	return std::make_pair(res, A);
}
ui big_integer::big_unsigned_cmp(const big_integer& a) const {
	if (digits.size() != a.digits.size()) {
		if (digits.size() > a.digits.size()) {
			return 1;
		}
		return 2;
	}
	for (size_t i = digits.size() - 1; ; i--) {
		if (digits[i] != a[i]) {
			if (digits[i] > a[i]) {
				return 1;
			}
			return 2;
		}
		if (i == 0) {
			break;
		}
	}
	return 0;
}
void big_integer::invert_bits() {
	for (size_t i = 0; i < digits.size(); i++) {
		digits[i] = ~digits[i];
	}
}

string to_string(const big_integer& arg) {
	return arg.get_decimal_string();
}

std::ostream& operator<< (std::ostream& os, const big_integer& data) {
	os << data.get_decimal_string();
	return os;
}
