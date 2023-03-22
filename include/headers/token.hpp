#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <functional>

namespace stone_lang {

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
	
public:
	std::function<TokenReturn()> eval;
	TokenReturn chain();

	Token(std::string text, std::function<TokenReturn()> eval_function): m_text(text), eval(eval_function) {};
};

} // namespace stone_lang

#endif