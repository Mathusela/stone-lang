#include "headers/expression.hpp"

#include "headers/parser.hpp"

#include <iostream>
namespace stone_lang {

std::string Expression::tokenize_subexpressions(std::string inputText) {
	int count=0;
	return replace_regex(inputText, "\\(((?:(?R)|[^\\(\\)}])*)\\)", [count]() mutable {return " %"+std::to_string(count++)+"% ";} );
}

std::vector<std::string> get_subexpressions_text(std::string inputText) {
	std::vector<std::string> out {};

	auto matches = get_all_regex_matches(inputText, "\\(((?:(?R)|[^\\(\\)}])*)\\)");
	for (auto match : matches) out.push_back(match[1]);

	return out;
}

std::string compress_spaces(std::string inputText) {
	return replace_regex(inputText, " +", " ");
}

std::vector<std::string> split_tokens(std::string inputText, std::string operators) {
	std::vector<std::string> out;
	// auto matches = get_all_regex_matches(inputText, "(?:[^ \\t"+operators+"]+)|["+operators+"]");
	auto matches = get_all_regex_matches(inputText, "(\")?(?(1)[^\"]+\"|[^ \t"+operators+"]+)|["+operators+"]");
	
	for (auto match : matches) out.push_back(match[0]);
	return out;
}

void Expression::init() {
	auto subexpressions = get_subexpressions_text(m_text);
	m_text = tokenize_subexpressions(m_text);
	m_text = compress_spaces(m_text);

	for (auto subexpression : subexpressions) m_subexpressions.push_back(Expression(subexpression));

	auto tokens = split_tokens(m_text, "=+\\-\\/*");
	for (auto token : tokens) m_tokens.push_back(create_token(token, this));

	// std::cout << m_subexpressions.size();
	// get_all_regex_matches(m_text, )
	std::cout << ">> \"" << m_text << "\"\n";
	for (auto token : m_tokens) {
		std::cout << ":: \"" << token.get_text() << "\" ";
		token.eval();
		std::cout << "\n";
	}
}

std::string Expression::get_text() {
	return m_text;
}

std::vector<Expression> Expression::get_subexpressions() {
	return m_subexpressions;
}

} // namespace stone_lang