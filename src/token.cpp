#include "headers/token.hpp"

#include "headers/parser.hpp"
#include "headers/token_evals.hpp"

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

Token* create_token(std::string tokenString, Expression* expression) {
	std::function<Generic(Token*)> evalFunc;
	TokenType type;

	if (is_type(tokenString)) {
		evalFunc = type_eval;
		type = TYPE;
	} else if (is_scope(tokenString)) {
		evalFunc = scope_eval;
		type = SCOPE;
	} else if (is_expression(tokenString)) {
		evalFunc = expression_eval;
		type = EXPRESSION;
	} else if (is_operator(tokenString)) {
		evalFunc = operator_eval;
		type = OPERATOR;
	} else if (is_literal(tokenString)) {
		evalFunc = literal_eval;
		type = LITERAL;
	} else {
		evalFunc = name_eval;
		type = NAME;
	}

	return new Token(tokenString, evalFunc, type, expression);
}

std::string Token::get_text() {
	return m_text;
}

Expression* Token::get_expression() {
	return m_expression;
}

void Token::set_prev_token(Token* token) {
	m_prevToken = token;
}

void Token::set_next_token(Token* token) {
	m_nextToken = token;
}

Token* Token::get_prev_token() {
	return m_prevToken;
}

Token* Token::get_next_token() {
	return m_nextToken;
}

void Token::set_literal_val_ptr(void* val) {
	delete_literal_val_ptr();
	m_literalValPtr = val;
}

void Token::delete_literal_val_ptr() {
	if (m_literalValPtr != nullptr) delete m_literalValPtr;
}

void* Token::get_literal_val_ptr() {
	return m_literalValPtr;
}

TokenType Token::get_token_type() {
	return m_tokenType;
}

} // namespace stone_lang