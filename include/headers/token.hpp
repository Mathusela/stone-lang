#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <functional>

#include "headers/types.hpp"

#include <iostream>

namespace stone_lang {

class Expression;

class Token {
private:
	std::string m_text;
	Token* m_prevToken;
	Token* m_nextToken;
	std::function<Generic(Token*)> m_eval;

	Expression* m_expression;
	void* m_literalValPtr = nullptr;

	TokenType m_tokenType;
	
public:
	Generic eval() {return m_eval(this);}

	std::string get_text();
	Expression* get_expression();

	void set_prev_token(Token* token);
	void set_next_token(Token* token);
	Token* get_prev_token();
	Token* get_next_token();

	void set_literal_val_ptr(void* val);
	void delete_literal_val_ptr();
	void* Token::get_literal_val_ptr();

	TokenType get_token_type();

	Token(std::string text, std::function<Generic(Token*)> eval_function, TokenType tokenType, Expression* expression): m_text(text), m_eval(eval_function), m_tokenType(tokenType) , m_expression(expression) {};
	~Token() {delete_literal_val_ptr();}
};

Token* create_token(std::string tokenString, Expression* expression);

} // namespace stone_lang

#endif