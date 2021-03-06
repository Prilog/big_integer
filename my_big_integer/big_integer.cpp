#include "big_integer.h"

#include <cstdio>
#include <vector>
#include <string>

typedef unsigned int ui;
typedef unsigned long long ull;

using std::vector;
using std::string;

const ull base = 4294967296;
const ui max_ui = 4294967295;

big_integer::big_integer() {
	size = 1;
	signum = 0;
	digits.clear();
	digits.push_back(0);
}

big_integer::~big_integer() {
	for (size_t i = 0; i < digits.size(); i++) {
		digits.clear();
	}
}

big_integer::big_integer(int arg) {
	size = 1;
	digits.clear();
	digits.push_back(abs(arg));
	if (arg >= 0) {
		signum = 0;
	}
	else {
		signum = 1;
	}
}

big_integer::big_integer(unsigned int arg) {
	size = 1;
	signum = 0;
	digits.clear();
	digits.push_back(arg);
}

big_integer::big_integer(ull arg) {
	size = 2;
	signum = 0;
	digits.clear();
	digits.push_back(ui(arg % base));
	digits.push_back(ui(arg / base));
	delete_space();
}

big_integer::big_integer(const big_integer& arg) {
	size = arg.size;
	signum = arg.signum;
	digits.clear();
	digits = arg.digits;
}

big_integer::big_integer(const string& arg) {
	size = 1;
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
	size = 1;
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
	for (size_t i = size - 1; ; i--) {
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
	reverse(res.begin(), res.end());
	return res;
}

big_integer& big_integer::operator= (const big_integer& arg) {
	size = arg.size;
	signum = arg.signum;
	digits.clear();
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
	a.signum = 0;
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
	a.signum = 0;
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
	for (int i = 0; i < digits.size(); i++) {
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
	if (signum == arg.signum) {
		res.second.signum = 0;
	}
	else {
		res.second.signum = 1;
	}
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

void big_integer::add_digit() {
	digits.push_back(0);
	size++;
}

void big_integer::delete_digit() {
	digits.pop_back();
	size--;
}

void big_integer::delete_space() {
	while (size > 1 && digits[size - 1] == 0) {
		digits.pop_back();
		size--;
	}
}

bool big_integer::is_zero() const {
	return size == 1 && digits[0] == 0;
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
		res.add_digit();
		res[i - from] = digits[i];
	}
	return res;
}

ui big_integer::small_unsigned_mod(const big_integer& a, ui b) {
	ull res = 0;
	ull power = 1;
	ull mod = b;
	for (size_t i = 0; i < a.size; i++) {
		res = (res + (ull(a[i]) % mod) * power) % mod;
		power = (power << 32) % mod;
	}
	return ui(res);
}

void big_integer::small_unsigned_sum(big_integer& a, ui b, size_t pos) {
	if (pos == a.size) {
		a.add_digit();
	}
	ull cur = ull(a[pos]) + ull(b);
	for (size_t i = pos; i < a.size; i++) {
		if (cur >= base) {
			a[i] = cur << 32 >> 32;
			if (i + 1 == a.size) {
				a.add_digit();
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
	for (size_t i = 0; i < a.size; i++) {
		ull cur = a[i] * ull(b);
		if (cur >= base) {
			small_unsigned_sum(res, cur << 32 >> 32, i);
			small_unsigned_sum(res, ui(cur / base), i + 1);
		}
		else {
			small_unsigned_sum(res, cur, i);
		}
	}
	a = res;
}

void big_integer::small_unsigned_div(big_integer& a, ui b) {
	if (a.size == 1) {
		a[0] = a[0] / b;
		return;
	}
	big_integer res(0);
	res.signum = a.signum;
	for (size_t i = 1; i < a.size; i++) {
		res.add_digit();
	}
	ull cur = a[a.size - 1];
	for (size_t i = a.size - 2; ; i--) {
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
	for (size_t i = pos; i < a.size; i++) {
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
	for (size_t i = 0; i < a.size; i++) {
		small_unsigned_sum(*this, a[i], i);
	}
}

void big_integer::big_unsigned_sub(const big_integer& a) {
	for (size_t i = 0; i < a.size; i++) {
		small_unsigned_sub(*this, a[i], i);
	}
}

void big_integer::big_unsigned_sub(const big_integer& a, size_t from) {
	for (size_t i = 0; i < a.size; i++) {
		small_unsigned_sub(*this, a[i], i + from);
	}
}

void big_integer::big_unsigned_mul(const big_integer& a) {
	big_integer res;
	res.signum = signum;
	for (size_t i = 0; i < size; i++) {
		for (size_t j = 0; j < a.size; j++) {
			ull x = ull(digits[i]) * ull(a[j]);
			small_unsigned_sum(res, (x << 32 >> 32), i + j);
			small_unsigned_sum(res, (x >> 32), i + j + 1);
		}
	}
	*this = res;
}
std::pair<big_integer, big_integer> big_integer::big_unsigned_div(const big_integer& a, const big_integer& arg) {
	if (arg.size <= 1) {
		big_integer x = a;
		ui m = small_unsigned_mod(x, arg[0]);
		small_unsigned_div(x, arg[0]);
		return std::make_pair(big_integer(1), big_integer(m));
	}
	ui cmp_res = a.big_unsigned_cmp(arg);
	if (cmp_res == 0) {
		return std::make_pair(big_integer(1), big_integer(0));
	}
	if (cmp_res == 2) {
		return std::make_pair(big_integer(0), a);
	}
	size_t n = arg.size;
	big_integer A = a;
	big_integer B = arg;
	big_integer res = 0;
	ui scale = ui(base / (ull(B[n - 1]) + 1));
	if (scale > 1) {
		small_unsigned_multiply(A, scale);
		small_unsigned_multiply(B, scale);
	}
	ui m = A.size - B.size;
	for (size_t i = 0; i < m; i++) {
		res.add_digit();
	}
	for (size_t i = m; ; i--) {
		size_t j = n + i;
		ull q = 1;
		if (i == m) {
			j = n + m - 1;
			q = ull(A[j]) / ull(B[n - 1]);
		}
		else {
			q = (ull(A[j]) * base + ull(A[j - 1])) / ull(B[n - 1]);
		}
		if (q > max_ui) {
			q = max_ui;
		}
		big_integer sub_int = A.sub_big_integer(i, j);
		big_integer mult = B;
		small_unsigned_multiply(mult, ui(q));
		while (mult > sub_int) {
			mult -= B;
			q--;
		}
		small_unsigned_sum(res, q, i);
		A.big_unsigned_sub(mult, i);
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
	res.delete_space();
	return std::make_pair(res, A);
}
ui big_integer::big_unsigned_cmp(const big_integer& a) const {
	if (size != a.size) {
		if (size > a.size) {
			return 1;
		}
		return 2;
	}
	for (size_t i = size - 1; ; i--) {
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

