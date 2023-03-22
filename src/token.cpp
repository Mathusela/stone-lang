#include "headers/token.hpp"

#include <iostream>

#include "headers/expression.hpp"
#include "headers/parser.hpp"

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
	return or_many<std::string>(tokenString, "int", "float", "bool", "none", "string");
}

bool is_operator(std::string tokenString) {
	return or_many<std::string>(tokenString, "=", "+", "-", "*", "/");
}

bool is_scope(std::string tokenString) {
	return tokenString[0] == '$';
}

bool is_expression(std::string tokenString) {
	return tokenString[0] == '%';
}

bool is_literal(std::string tokenString) {
	return is_regex_match(tokenString, "(?:(?<![$%])\\d+(?:.\\d+)?(?![$%]))|True|False|None|(?:\"[^\"]*\")");
}

TokenReturn type_eval(Token* token) {
	std::cout << "TYPE";
	return TokenReturn {};
}

TokenReturn name_eval(Token* token) {
	std::cout << "NAME";
	return TokenReturn {};
}

TokenReturn scope_eval(Token* token) {
	std::cout << "SCOPE";
	return TokenReturn {};
}

TokenReturn expression_eval(Token* token) {
	std::cout << "EXPRESSION: " << (token->get_expression()->get_subexpressions()[ (int)(token->get_text()[1] - '0') ]).get_text();
	return TokenReturn {};
}

TokenReturn operator_eval(Token* token) {
	std::cout << "OPERATOR";
	return TokenReturn {};
}

TokenReturn keyword_eval(Token* token) {
	std::cout << "KEYWORD";
	return TokenReturn {};
}

TokenReturn literal_eval(Token* token) {
	std::cout << "LITERAL: " << (token->get_text());
	return TokenReturn {};
}

// Maybe should return pointer?
Token create_token(std::string tokenString, Expression* expression) {
	std::function<TokenReturn(Token*)> evalFunc;

	if (is_type(tokenString)) {
		evalFunc = type_eval;
	} else if (is_scope(tokenString)) {
		evalFunc = scope_eval;
	} else if (is_expression(tokenString)) {
		evalFunc = expression_eval;
	} else if (is_operator(tokenString)) {
		evalFunc = operator_eval;
	} else if (is_literal(tokenString)) {
		evalFunc = literal_eval;
	} else {
		evalFunc = name_eval;
	}

	return Token(tokenString, evalFunc, expression);
}

std::string Token::get_text() {
	return m_text;
}

Expression* Token::get_expression() {
	return m_expression;
}

} // namespace stone_lang