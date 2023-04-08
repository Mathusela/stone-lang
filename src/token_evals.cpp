#include "headers/token_evals.hpp"

#include <type_traits>

#include "headers/token.hpp"
#include "headers/scope.hpp"
#include "headers/parser.hpp"

#define _CALL_DOUBLE_TEMPLATED_FUNCTION_ON_TOKEN_TYPES_(prevType, nextTokenType, function, out, ...)				\
	if (nextTokenType == INT) out = function##<prevType, t_int>(__VA_ARGS__);										\
	else if (nextTokenType == FLOAT) out = function##<prevType, t_float>(__VA_ARGS__);								\
	else if (nextTokenType == NONE) out = Generic{};																\
	else if (nextTokenType == STRING) out = function##<prevType, t_string>(__VA_ARGS__);							\
	else if (nextTokenType == BOOL) out = function##<prevType, t_bool>(__VA_ARGS__);								\
	else if (nextTokenType == _SCOPE_PTR) out = Generic{};															\
	else out = Generic{};

#define CALL_DOUBLE_TEMPLATED_FUNCTION_ON_TOKEN_TYPES(prevTokenType, nextTokenType, function, out, ...)													\
	Generic out;																																		\
	if (prevTokenType == INT) {_CALL_DOUBLE_TEMPLATED_FUNCTION_ON_TOKEN_TYPES_(t_int, nextTokenType, function, out, __VA_ARGS__)}						\
	else if (prevTokenType == FLOAT) {_CALL_DOUBLE_TEMPLATED_FUNCTION_ON_TOKEN_TYPES_(t_float, nextTokenType, function, out, __VA_ARGS__)}				\
	else if (prevTokenType == NONE) out = Generic{};																									\
	else if (prevTokenType == STRING) {_CALL_DOUBLE_TEMPLATED_FUNCTION_ON_TOKEN_TYPES_(t_string, nextTokenType, function, out, __VA_ARGS__)}			\
	else if (prevTokenType == BOOL) {_CALL_DOUBLE_TEMPLATED_FUNCTION_ON_TOKEN_TYPES_(t_bool, nextTokenType, function, out, __VA_ARGS__)}				\
	else if (prevTokenType == _SCOPE_PTR) out = Generic{};																								\
	else out = Generic{};

namespace stone_lang {

template <typename T> struct type_to_enum_ {static constexpr InternalType val = NONE;};
template <> struct type_to_enum_<t_bool> {static constexpr InternalType val = BOOL;};
template <> struct type_to_enum_<t_float> {static constexpr InternalType val = FLOAT;};
template <> struct type_to_enum_<t_int> {static constexpr InternalType val = INT;};
template <> struct type_to_enum_<t_none> {static constexpr InternalType val = NONE;};
template <> struct type_to_enum_<t_string> {static constexpr InternalType val = STRING;};
template <> struct type_to_enum_<t_SCOPE_PTR> {static constexpr InternalType val = _SCOPE_PTR;};

// ====== TYPE EVAL ======
Generic type_eval(Token* token) {
	std::cout << "TYPE :" << token->get_text() << ";\n";

	InternalType type;
	auto str = token->get_text();

	if (str == "int") type = INT;
	else if (str == "float") type = FLOAT;
	else if (str == "bool") type = BOOL;
	else if (str == "string") type = STRING;
	else if (str == "none") type = NONE;
	else type = NONE; 

	return Generic {nullptr, type};
}
// =======================

// ====== NAME EVAL ======
// Debugging code
template <typename T, typename _>
Generic printVal(Generic* var) {
	std::cout << "VAL: " << *(T*)var->val << ";";
	return Generic{};
}
// --------------

Generic name_eval(Token* token) {

	auto prevToken = token->get_prev_token();
	if (prevToken != nullptr && prevToken->get_token_type() == TYPE) {
		token->get_expression()->get_scope()->initialize_var(token->get_text(), prevToken->eval().type);
	}

	// Debugging code
	std::cout << "NAME :" << token->get_text() << "; ";
	auto varPRINT = token->get_expression()->get_scope()->get_var(token->get_text());
	if (varPRINT != nullptr && varPRINT->val != nullptr) {
		CALL_DOUBLE_TEMPLATED_FUNCTION_ON_TOKEN_TYPES(varPRINT->type, INT, printVal, _, varPRINT);
	}
	std::cout << "\n";
	// --------------

	auto var = token->get_expression()->get_scope()->get_var(token->get_text());
	return Generic {var->val, var->type};
}
// =======================

// ====== SCOPE_EVAL ======
Generic scope_eval(Token* token) {
	std::cout << "SCOPE :" << token->get_text() << ";\n";
	return Generic {};
}
// ========================

// ====== EXPRESSION_EVAL ======
Generic expression_eval(Token* token) {
	std::cout << "EXPRESSION :" << token->get_text() << ";\n";

	return token->get_expression()->get_subexpressions()[std::stoi(get_all_regex_matches(token->get_text(), "%(\\d+)%")[0][1])]->eval();
}
// =============================

// ====== OPERATOR_EVAL ======
// --- TYPE INTROSPECTION ---
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

template <typename T1, typename T2, typename U = int>
struct HasCast_ : std::false_type {};
template <typename T1, typename T2>
struct HasCast_<T1, T2, decltype((T2)std::declval<T1>(), 0)> : std::true_type {};

// --------------------------

// --- OPERATOR ACTIONS ---
template <typename T1, typename T2>
typename std::enable_if<(bool)HasAdd_<T1, T2>(), Generic>::type add(Generic prevEval, Generic nextEval) {
	auto val = (void*)new decltype(std::declval<T1>() + std::declval<T2>())( *(T1*)prevEval.val + *(T2*)nextEval.val );
	auto type = type_to_enum_<decltype(std::declval<T1>() + std::declval<T2>())>::val;
	return Generic{val, type};
}
template <typename T1, typename T2>
typename std::enable_if<!(bool)HasAdd_<T1, T2>(), Generic>::type add(Generic prevEval, Generic nextEval) {
	return Generic{};
}

template <typename T1, typename T2>
typename std::enable_if<(bool)HasSub_<T1, T2>(), Generic>::type sub(Generic prevEval, Generic nextEval) {
	auto val = (void*)new decltype(std::declval<T1>() - std::declval<T2>())( *(T1*)prevEval.val - *(T2*)nextEval.val );
	auto type = type_to_enum_<decltype(std::declval<T1>() - std::declval<T2>())>::val;
	return Generic{val, type};
}
template <typename T1, typename T2>
typename std::enable_if<!(bool)HasSub_<T1, T2>(), Generic>::type sub(Generic prevEval, Generic nextEval) {
	return Generic{};
}

template <typename T1, typename T2>
typename std::enable_if<(bool)HasMul_<T1, T2>(), Generic>::type mul(Generic prevEval, Generic nextEval) {
	auto val = (void*)new decltype(std::declval<T1>() * std::declval<T2>())( *(T1*)prevEval.val * *(T2*)nextEval.val );
	auto type = type_to_enum_<decltype(std::declval<T1>() * std::declval<T2>())>::val;
	return Generic{val, type};
}
template <typename T1, typename T2>
typename std::enable_if<!(bool)HasMul_<T1, T2>(), Generic>::type mul(Generic prevEval, Generic nextEval) {
	return Generic{};
}

template <typename T1, typename T2>
typename std::enable_if<(bool)HasDiv_<T1, T2>(), Generic>::type div(Generic prevEval, Generic nextEval) {
	auto val = (void*)new decltype(std::declval<T1>() / std::declval<T2>())( *(T1*)prevEval.val / *(T2*)nextEval.val );
	auto type = type_to_enum_<decltype(std::declval<T1>() / std::declval<T2>())>::val;
	return Generic{val, type};
}
template <typename T1, typename T2>
typename std::enable_if<!(bool)HasDiv_<T1, T2>(), Generic>::type div(Generic prevEval, Generic nextEval) {
	return Generic{};
}

template <typename VarType, typename ValType>
typename std::enable_if<(bool)HasCast_<ValType, VarType>(), Generic>::type assign(Generic* var, void* val) {
	if (val != nullptr) var->val = (void*)new VarType((VarType)*(ValType*)val);
	return Generic{};
}
template <typename VarType, typename ValType>
typename std::enable_if<!(bool)HasCast_<ValType, VarType>(), Generic>::type assign(Generic* var, void* val) {
	std::cout << "TYPE ERROR" << "\n";
	return Generic{};
}
// ------------------------

Generic operator_eval(Token* token) {
	std::cout << "OPERATOR :" << token->get_text() << ";\n";

	auto tokenText = token->get_text();
	auto prevToken = token->get_prev_token();
	auto nextToken = token->get_next_token();
	auto prevEval = prevToken->eval();
	auto nextEval = nextToken->eval();
	if (tokenText == "=") {
		auto var = prevToken->get_expression()->get_scope()->get_var(prevToken->get_text());
		if (var->val != nullptr) delete var->val;
		CALL_DOUBLE_TEMPLATED_FUNCTION_ON_TOKEN_TYPES(prevEval.type, nextEval.type, assign, _, var, nextEval.val);
		return Generic {var->val, var->type};
	} else if (tokenText == "+") {
		CALL_DOUBLE_TEMPLATED_FUNCTION_ON_TOKEN_TYPES(prevEval.type, nextEval.type, add, val, prevEval, nextEval);
		token->set_literal_val_ptr(val.val);
		return val;
	} else if (tokenText == "-") {
		CALL_DOUBLE_TEMPLATED_FUNCTION_ON_TOKEN_TYPES(prevEval.type, nextEval.type, sub, val, prevEval, nextEval);
		token->set_literal_val_ptr(val.val);
		return val;
	} else if (tokenText == "*") {
		CALL_DOUBLE_TEMPLATED_FUNCTION_ON_TOKEN_TYPES(prevEval.type, nextEval.type, mul, val, prevEval, nextEval);
		token->set_literal_val_ptr(val.val);
		return val;
	} else if (tokenText == "/") {
		CALL_DOUBLE_TEMPLATED_FUNCTION_ON_TOKEN_TYPES(prevEval.type, nextEval.type, div, val, prevEval, nextEval);
		token->set_literal_val_ptr(val.val);
		return val;
	} else {
		return Generic {};
	}
}
// ===========================

// ====== LITERAL EVAL ======
bool is_int_literal(std::string tokenText) {
	return is_regex_match(tokenText, "^\\d+$");
}

bool is_string_literal(std::string tokenText) {
	return is_regex_match(tokenText, "^\"[^\"]+\"$");
}

bool is_float_literal(std::string tokenText) {
	return is_regex_match(tokenText, "^\\d*\\.\\d*$");
}

Generic literal_eval(Token* token) {
	std::cout << "LITERAL :" << token->get_text() << ";\n";

	auto literalValPtr = token->get_literal_val_ptr();

	auto tokenText = token->get_text();

	if (is_int_literal(tokenText)) {
		if (literalValPtr) return Generic{literalValPtr, INT}; 
		void* val = (void*)new int(std::stoi(tokenText));
		token->set_literal_val_ptr(val);
		return Generic {val, INT};
	} else if (is_string_literal(tokenText)) {
		if (literalValPtr) return Generic{literalValPtr, STRING};
		void* val = (void*)new std::string(tokenText.substr(1, tokenText.size()-2));
		token->set_literal_val_ptr(val);
		return Generic {val, STRING};
	} else if (is_float_literal(tokenText)) {
		if (literalValPtr) return Generic{literalValPtr, FLOAT};
		void* val = (void*)new double(std::stod(tokenText));
		token->set_literal_val_ptr(val);
		return Generic {val, FLOAT};
	}
	else {
		return Generic {};
	}
}
// ==========================

} // namespace stone_lang