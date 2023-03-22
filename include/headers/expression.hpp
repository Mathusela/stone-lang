#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <vector>

#include "headers/token.hpp"

namespace stone_lang {

class Expression {
private:
	std::vector<Expression> subExpressions {};
	std::vector<Token> tokens {};

public:
	Expression();
};

} // namespace stone_lang

#endif