#ifndef SCOPE_HPP
#define SCOPE_HPP

#include <vector>
#include <unordered_map>
#include <string>

#include <iostream>

#include "headers/expression.hpp"
#include "headers/token.hpp"

namespace stone_lang {

class Scope {
private:
	std::string m_name;
	std::string m_text;
	std::vector<Expression*> m_expressions {};
	std::vector<Scope*> m_subscopes {};
	std::unordered_map<std::string, Generic*> m_vars {};

	void init();

public:
	Scope(std::string text, std::string name): m_text(text), m_name(name) {init();}
	~Scope() {
		for (auto var : m_vars) {delete var.second->val;}
		for (auto var : m_vars) {delete var.second;}
		for (auto scope : m_subscopes) delete scope;
		for (auto expression : m_expressions) delete expression;
	}

	void eval();

	std::string get_name();
	std::string get_text();
	std::vector<Expression*> get_subexpressions();
	Generic* get_var(std::string name);
	void Scope::initialize_var(std::string name, InternalType type);
};

} // namespace stone_lang

#endif SCOPE_HPP