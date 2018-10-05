#pragma once

#include <cstdio>
#include <vector>
#include <memory>

typedef unsigned int ui;
typedef unsigned long long ull;

class neko_vector {
private:
	const ull base = 4294967296;
	const ui max_ui = 4294967295;
	const int min_int = -int(2147483648);
	const int max_int = 2147483647;
	size_t sz;
	ui buf;
	std::shared_ptr <std::vector <ui> > arr;

	void copy_vector();
	void create_vector();
	void delete_vector();
	
public:
	neko_vector();
	~neko_vector();
	neko_vector(neko_vector const& arg);

	void push_back(const ui arg);
	void pop_back();
	size_t size() const;
	ui operator[](size_t index) const;
	ui& operator[](size_t index);
	neko_vector& operator=(neko_vector const &arg);
	void clear();

};