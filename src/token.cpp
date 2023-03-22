#include "headers/token.hpp"

namespace stone_lang {

template <typename T, typename... TP>
bool or_many(T X, T y, TP... yp) {
	return X == y || or_many<T>(X, yp...);
}
template <typename T>
bool or_many(T X, T y) {
	return X == y;
}

bool is_type(std::string tokenString) {
	return or_many<std::string>(tokenString, "int", "float", "bool");
}

bool is_scope(std::string tokenString) {
	return tokenString[0] == '$';
}


TokenReturn type_eval() {
	return TokenReturn {};
}

TokenReturn name_eval() {
	return TokenReturn {};
}

TokenReturn scope_eval() {
	return TokenReturn {};
}

TokenReturn keyword_eval() {
	return TokenReturn {};
}

// Maybe should return pointer?
Token create_token(std::string tokenString) {
	std::function<TokenReturn()> evalFunc;

	if (is_type(tokenString)) {
		evalFunc = type_eval;
	} else if (is_scope(tokenString)) {
		evalFunc = name_eval;
	} else {
		evalFunc = name_eval;
	}

	return Token(tokenString, evalFunc);
}

} // namespace stone_lang