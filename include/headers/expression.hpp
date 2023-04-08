#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <vector>
#include <string>

#include "headers/token.hpp"

namespace stone_lang {

class Scope;

class Expression {
private:
	std::string m_text;
	std::vector<Expression*> m_subexpressions {};
	std::vector<Token*> m_tokens {};

	// std::string tokenize_subexpressions(std::string inputText);

	Scope* m_scope;

	void init();

public:
	Expression(std::string text, Scope* scope): m_text(text), m_scope(scope) {init();}
	~Expression() {
		for (auto expression : m_subexpressions) delete expression;
		for (auto token : m_tokens) delete token;
	}

	Generic eval();

	std::string get_text();
	std::vector<Expression*> get_subexpressions();
	Scope* get_scope();
};

} // namespace stone_lang

#endif