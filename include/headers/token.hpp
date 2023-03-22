#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <functional>

namespace stone_lang {

class Expression;

enum TokenReturnType {
	
};

struct TokenReturn {
	uint64_t val;
	TokenReturnType type; 
};

class Token {
private:
	std::string m_text;
	Token* m_prevToken;
	Token* m_nextToken;
	std::function<TokenReturn(Token*)> m_eval;

	Expression* m_expression;
	
public:
	TokenReturn chain();
	TokenReturn eval() {return m_eval(this);}

	std::string get_text();
	Expression* get_expression();

	Token(std::string text, std::function<TokenReturn(Token*)> eval_function, Expression* expression): m_text(text), m_eval(eval_function), m_expression(expression) {};
};

Token create_token(std::string tokenString, Expression* expression);

} // namespace stone_lang

#endif