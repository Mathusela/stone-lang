#ifndef SCOPE_HPP
#define SCOPE_HPP

#include <vector>
#include <unordered_map>
#include <string>

#include <iostream>

#include "headers/expression.hpp"
#include "headers/token.hpp"

namespace stone_lang {

using Generic = TokenReturn;
using GenericType = TokenReturnType;

class Scope {
private:
	std::string m_text;
	std::vector<Expression*> m_expressions {};
	std::vector<Scope*> m_subscopes {};
	std::unordered_map<std::string, Generic*> m_vars {};

	void init();

public:
	Scope(std::string text): m_text(text) {init();}
	~Scope() {
		for (auto var : m_vars) {delete var.second->val;}
		for (auto var : m_vars) {delete var.second;}
		for (auto scope : m_subscopes) delete scope;
		for (auto expression : m_expressions) delete expression;
	}

	void eval();

	std::string get_text();
	std::vector<Expression*> get_subexpressions();
	Generic* get_var(std::string name);
	void Scope::initialize_var(std::string name, GenericType type);
};

} // namespace stone_lang

#endif SCOPE_HPP