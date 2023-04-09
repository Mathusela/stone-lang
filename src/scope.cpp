#include "headers/scope.hpp"

#include <iostream>

#include "headers/parser.hpp"

namespace stone_lang {

std::vector<std::string> get_scopes(std::string inputText) {
	std::vector<std::string> out {};

	auto matches = get_all_regex_matches(inputText, "{((?:(?R)|[^{}])*)}");
	for (auto match : matches) out.push_back(match[1]);

	return out;
}

std::string tokenize_scopes(std::string inputText) {
	return replace_regex(inputText, "{((?:(?R)|[^{}])*)}", [](){static int count=0; return " \\$"+std::to_string(count++)+"\\$;";} );
}

void Scope::init() {
	std::cout << m_text << "\n";

	auto subscopes = get_scopes(m_text);
	m_text = tokenize_scopes(m_text);
	for (auto subscope : subscopes) m_subscopes.push_back(new Scope(subscope, "%NAME%"));
	
	auto expressionsText = get_expressions_from_text(m_text);
	for (auto expression : expressionsText) m_expressions.push_back(new Expression(expression, this));
}

void Scope::eval() {
	for (auto expression : m_expressions) expression->eval();
}

std::string Scope::get_name() {
	return m_name;
}

std::string Scope::get_text() {
	return m_text;
}

std::vector<Expression*> Scope::get_subexpressions() {
	return m_expressions;
}

Generic* Scope::get_var(std::string name) {
	if (m_vars.find(name) != m_vars.end()) return m_vars[name];
	else {
		throw std::runtime_error("ERROR in scope \"" + m_name + "\": Uninitialized variable \"" + name + "\"");
		return nullptr;
	}
}

void Scope::initialize_var(std::string name, InternalType type) {
	m_vars[name] = new Generic{nullptr, type};
}

} // namespace stone_lang