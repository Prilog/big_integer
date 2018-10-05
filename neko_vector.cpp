#include "neko_vector.h"
#include <cstdio>

typedef unsigned int ui;
typedef unsigned long long ull;

neko_vector::neko_vector() {
	buf = 0;
	sz = 0;
	own = true;
	arr.reset();
}

neko_vector::~neko_vector() {
	if (arr) {
		arr.reset();
	}
}

void neko_vector::copy_vector() {
	if (own) {
		return;
	}
	own = true;
	std::vector <ui> x = *arr;
	arr = std::make_shared <std::vector <ui> >(x);
}

void neko_vector::create_vector() {
	std::vector <ui> new_one;
	new_one.clear();
	arr = std::make_shared<std::vector <ui>> (new_one);
}

void neko_vector::delete_vector() {
	arr.reset();
}

neko_vector::neko_vector(neko_vector const &arg) {
	sz = arg.sz;
	if (arg.sz > 1) {
		buf = 0;
		own = false;
		arr = arg.arr;
	}
	else {
		own = true;
		buf = arg.buf;;
		delete_vector();
	}
}

void neko_vector::push_back(const ui arg) {
	if (sz == 0) {
		buf = arg;
	} else if (sz == 1) {
		create_vector();
		arr->push_back(buf);
		arr->push_back(arg);
	}
	else {
		copy_vector();
		arr->push_back(arg);
	}
	sz++;
}

void neko_vector::pop_back() {
	if (sz == 1) {
		buf = 0;
	}
	else if (sz == 2) {
		buf = (*arr)[0];
		own = true;
		delete_vector();
	}
	else {
		copy_vector();
		arr->pop_back();
	}
	sz--;
}

size_t neko_vector::size() const {
	return sz;
}

ui neko_vector::operator[](size_t index) const {
	if (sz == 1) {
		return buf;
	}
	else {
		return (*arr)[index];
	}
}

ui& neko_vector::operator[](size_t index) {
	if (sz == 1) {
		return buf;
	}
	else {
		copy_vector();
		return (*arr)[index];
	}
}
neko_vector& neko_vector::operator=(neko_vector const &arg) {
	if (arg.size() < 2) {
		sz = arg.sz;
		buf = arg.buf;
	}
	else {
		copy_vector();
		sz = arg.sz;
		buf = 0;
		arr = arg.arr;
	}
	return *this;
}

void neko_vector::clear() {
	sz = 0;
	buf = 0;
	own = true;
	delete_vector();
}
