#include "headers/token.hpp"

#include <iostream>
#include <type_traits>

#include "headers/scope.hpp"
#include "headers/expression.hpp"
#include "headers/parser.hpp"

#define _CALL_TEMPLATED_FUNCTION_BASED_ON_TOKEN_TYPE_(prevType, nextTokenType, function, out, ...)					\
	if (nextTokenType == INT) out = function##<prevType, int>(__VA_ARGS__);											\
	else if (nextTokenType == FLOAT) out = function##<prevType, double>(__VA_ARGS__);								\
	else if (nextTokenType == NONE) out = nullptr;																	\
	else if (nextTokenType == STRING) out = function##<prevType, std::string>(__VA_ARGS__);							\
	else if (nextTokenType == BOOL) out = function##<prevType, bool>(__VA_ARGS__);									\
	else if (nextTokenType == _SCOPE_PTR) out = nullptr;															\
	else out = nullptr;

#define CALL_TEMPLATED_FUNCTION_BASED_ON_TOKEN_TYPE(prevTokenType, nextTokenType, function, out, ...)													\
	void* out;																																			\
	if (prevTokenType == INT) {_CALL_TEMPLATED_FUNCTION_BASED_ON_TOKEN_TYPE_(int, nextTokenType, function, out, __VA_ARGS__)}							\
	else if (prevTokenType == FLOAT) {_CALL_TEMPLATED_FUNCTION_BASED_ON_TOKEN_TYPE_(double, nextTokenType, function, out, __VA_ARGS__)}					\
	else if (prevTokenType == NONE) out = nullptr;																										\
	else if (prevTokenType == STRING) {_CALL_TEMPLATED_FUNCTION_BASED_ON_TOKEN_TYPE_(std::string, nextTokenType, function, out, __VA_ARGS__)}			\
	else if (prevTokenType == BOOL) {_CALL_TEMPLATED_FUNCTION_BASED_ON_TOKEN_TYPE_(bool, nextTokenType, function, out, __VA_ARGS__)}					\
	else if (prevTokenType == _SCOPE_PTR) out = nullptr;																								\
	else out = nullptr;

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
	std::cout << "TYPE :" << token->get_text() << ";\n";

	TokenReturnType type;
	auto str = token->get_text();

	if (str == "int") type = INT;
	else if (str == "float") type = FLOAT;
	else if (str == "bool") type = BOOL;
	else if (str == "string") type = STRING;
	else if (str == "none") type = NONE;
	else type = NONE; 

	return TokenReturn {nullptr, type};
}

template <typename T, typename _>
void* printVal(Generic* var) {
	std::cout << "VAL: " << *(T*)var->val << ";\n";
	std::cout << "\n";
	return nullptr;
}

TokenReturn name_eval(Token* token) {
	std::cout << "NAME :" << token->get_text() << "; ";
	auto varPRINT = token->get_expression()->get_scope()->get_var(token->get_text());
	if (varPRINT != nullptr && varPRINT->val != nullptr) {
		CALL_TEMPLATED_FUNCTION_BASED_ON_TOKEN_TYPE(varPRINT->type, INT, printVal, _, varPRINT);
	}

	auto prevToken = token->get_prev_token();
	if (prevToken != nullptr && prevToken->get_token_type() == TYPE) {
		token->get_expression()->get_scope()->initialize_var(token->get_text(), prevToken->eval().type);
	}

	auto var = token->get_expression()->get_scope()->get_var(token->get_text());
	return TokenReturn {var->val, var->type};
}

TokenReturn scope_eval(Token* token) {
	std::cout << "SCOPE :" << token->get_text() << ";\n";
	return TokenReturn {};
}

TokenReturn expression_eval(Token* token) {
	std::cout << "EXPRESSION :" << token->get_text() << ";\n";

	return token->get_expression()->get_subexpressions()[std::stoi(get_all_regex_matches(token->get_text(), "%(\\d+)%")[0][1])]->eval();
}

template <typename T1, typename T2, typename U = int>
struct HasAdd_ : std::false_type {};
template <typename T1, typename T2>
struct HasAdd_<T1, T2, decltype(std::declval<T1>() + std::declval<T2>(), 0)> : std::true_type {};

template <typename T1, typename T2, typename U = int>
struct HasSub_ : std::false_type {};
template <typename T1, typename T2>
struct HasSub_<T1, T2, decltype(std::declval<T1>() - std::declval<T2>(), 0)> : std::true_type {};

template <typename T1, typename T2, typename U = int>
struct HasMul_ : std::false_type {};
template <typename T1, typename T2>
struct HasMul_<T1, T2, decltype(std::declval<T1>() * std::declval<T2>(), 0)> : std::true_type {};

template <typename T1, typename T2, typename U = int>
struct HasDiv_ : std::false_type {};
template <typename T1, typename T2>
struct HasDiv_<T1, T2, decltype(std::declval<T1>() / std::declval<T2>(), 0)> : std::true_type {};

template <typename T1, typename T2>
typename std::enable_if<(bool)HasAdd_<T1, T2>(), void*>::type add(TokenReturn prevEval, TokenReturn nextEval) {
	return (void*)new decltype(std::declval<T1>() + std::declval<T2>())( *(T1*)prevEval.val + *(T2*)nextEval.val );
}
template <typename T1, typename T2>
typename std::enable_if<!(bool)HasAdd_<T1, T2>(), void*>::type add(TokenReturn prevEval, TokenReturn nextEval) {
	return nullptr;
}

template <typename T1, typename T2>
typename std::enable_if<(bool)HasSub_<T1, T2>(), void*>::type sub(TokenReturn prevEval, TokenReturn nextEval) {
	return (void*)new decltype(std::declval<T1>() - std::declval<T2>())( *(T1*)prevEval.val - *(T2*)nextEval.val );
}
template <typename T1, typename T2>
typename std::enable_if<!(bool)HasSub_<T1, T2>(), void*>::type sub(TokenReturn prevEval, TokenReturn nextEval) {
	return nullptr;
}

template <typename T1, typename T2>
typename std::enable_if<(bool)HasMul_<T1, T2>(), void*>::type mul(TokenReturn prevEval, TokenReturn nextEval) {
	return (void*)new decltype(std::declval<T1>() * std::declval<T2>())( *(T1*)prevEval.val * *(T2*)nextEval.val );
}
template <typename T1, typename T2>
typename std::enable_if<!(bool)HasMul_<T1, T2>(), void*>::type mul(TokenReturn prevEval, TokenReturn nextEval) {
	return nullptr;
}

template <typename T1, typename T2>
typename std::enable_if<(bool)HasDiv_<T1, T2>(), void*>::type div(TokenReturn prevEval, TokenReturn nextEval) {
	return (void*)new decltype(std::declval<T1>() / std::declval<T2>())( *(T1*)prevEval.val / *(T2*)nextEval.val );
}
template <typename T1, typename T2>
typename std::enable_if<!(bool)HasDiv_<T1, T2>(), void*>::type div(TokenReturn prevEval, TokenReturn nextEval) {
	return nullptr;
}

template <typename T1, typename T2>
void* assign(TokenReturn& nextEval, Generic* var) {
	// TODO: new T1, cast to T2
	if (nextEval.val != nullptr) var->val = (void*)new T2(*(T2*)nextEval.val);
	return nullptr;
}

TokenReturn operator_eval(Token* token) {
	std::cout << "OPERATOR :" << token->get_text() << ";\n";

	auto tokenText = token->get_text();
	auto prevToken = token->get_prev_token();
	auto nextToken = token->get_next_token();
	auto prevEval = prevToken->eval();
	auto nextEval = nextToken->eval();
	if (tokenText == "=") {
		if (prevEval.type != nextEval.type) std::cout << "TYPE ERROR" << "\n";
		auto var = prevToken->get_expression()->get_scope()->get_var(prevToken->get_text());
		if (var->val != nullptr) delete var->val;
		CALL_TEMPLATED_FUNCTION_BASED_ON_TOKEN_TYPE(prevEval.type, nextEval.type, assign, _ ,nextEval, var);
		return TokenReturn {var->val, var->type};
	} else if (tokenText == "+") {
		CALL_TEMPLATED_FUNCTION_BASED_ON_TOKEN_TYPE(prevEval.type, nextEval.type, add, val, prevEval, nextEval);
		token->set_literal_val_ptr(val);
		return TokenReturn{val, prevEval.type};
	} else if (tokenText == "-") {
		CALL_TEMPLATED_FUNCTION_BASED_ON_TOKEN_TYPE(prevEval.type, nextEval.type, sub, val, prevEval, nextEval);
		token->set_literal_val_ptr(val);
		return TokenReturn{val, prevEval.type};
	} else if (tokenText == "*") {
		CALL_TEMPLATED_FUNCTION_BASED_ON_TOKEN_TYPE(prevEval.type, nextEval.type, mul, val, prevEval, nextEval);
		token->set_literal_val_ptr(val);
		return TokenReturn{val, prevEval.type};
	} else if (tokenText == "/") {
		CALL_TEMPLATED_FUNCTION_BASED_ON_TOKEN_TYPE(prevEval.type, nextEval.type, div, val, prevEval, nextEval);
		token->set_literal_val_ptr(val);
		return TokenReturn{val, prevEval.type};
	} else {
		return TokenReturn {};
	}
}

TokenReturn keyword_eval(Token* token) {
	std::cout << "KEYWORD :" << token->get_text() << ";\n";
	return TokenReturn {};
}

bool is_int_literal(std::string tokenText) {
	return is_regex_match(tokenText, "^\\d+$");
}

bool is_string_literal(std::string tokenText) {
	return is_regex_match(tokenText, "^\"[^\"]+\"$");
}

bool is_float_literal(std::string tokenText) {
	return is_regex_match(tokenText, "^\\d*\\.\\d*$");
}

TokenReturn literal_eval(Token* token) {
	std::cout << "LITERAL :" << token->get_text() << ";\n";

	auto literalValPtr = token->get_literal_val_ptr();

	auto tokenText = token->get_text();

	if (is_int_literal(tokenText)) {
		if (literalValPtr) return TokenReturn{literalValPtr, INT}; 
		void* val = (void*)new int(std::stoi(tokenText));
		token->set_literal_val_ptr(val);
		return TokenReturn {val, INT};
	} else if (is_string_literal(tokenText)) {
		if (literalValPtr) return TokenReturn{literalValPtr, STRING};
		void* val = (void*)new std::string(tokenText.substr(1, tokenText.size()-2));
		token->set_literal_val_ptr(val);
		return TokenReturn {val, STRING};
	} else if (is_float_literal(tokenText)) {
		if (literalValPtr) return TokenReturn{literalValPtr, FLOAT};
		void* val = (void*)new double(std::stod(tokenText));
		token->set_literal_val_ptr(val);
		return TokenReturn {val, FLOAT};
	}
	else {
		return TokenReturn {};
	}
}

// Maybe should return pointer?
Token* create_token(std::string tokenString, Expression* expression) {
	std::function<TokenReturn(Token*)> evalFunc;
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