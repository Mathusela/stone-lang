#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <vector>
#include <string>

#include "headers/token.hpp"

namespace stone_lang {

class Expression {
private:
	std::string m_text;
	std::vector<Expression> m_subexpressions {};
	std::vector<Token> m_tokens {};

	std::string tokenize_subexpressions(std::string inputText);

	void init();

public:
	Expression(std::string text): m_text(text) {init();}

	std::string get_text();
	std::vector<Expression> get_subexpressions();
};

} // namespace stone_lang

#endif