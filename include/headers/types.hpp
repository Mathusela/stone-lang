#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>

namespace stone_lang {

class Scope;

enum TokenType {
	OPERATOR,
	SCOPE,
	NAME,
	LITERAL,
	EXPRESSION,
	TYPE
};

enum InternalType {
	INT,
	FLOAT,
	NONE,
	STRING,
	BOOL,
	_SCOPE_PTR
};

using t_int = int;
using t_float = double;
using t_none = void*;
using t_string = std::string;
using t_bool = bool;
using t_SCOPE_PTR = Scope*;

struct Generic {
	void* val = nullptr;
	InternalType type = NONE; 

	bool operator== (Generic x) {
		return type == x.type && val == x.val;
	}
};
} // namespace stone_lang

#endif