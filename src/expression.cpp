#include "headers/expression.hpp"

#include <algorithm>

#include "headers/parser.hpp"

#include <iostream>
namespace stone_lang {

std::string tokenize_subexpressions(std::string inputText) {
	int count=0;
	return replace_regex(inputText, "\\(((?:(?R)|[^\\(\\)])*)\\)", [count]() mutable {return " %"+std::to_string(count++)+"% ";} );
}

std::vector<std::string> get_subexpressions_text(std::string inputText) {
	std::vector<std::string> out {};

	auto matches = get_all_regex_matches(inputText, "\\(((?:(?R)|[^\\(\\)])*)\\)");
	for (auto match : matches) out.push_back(match[1]);

	return out;
}

std::string compress_spaces(std::string inputText) {
	return replace_regex(inputText, " +", " ");
}

std::vector<std::string> split_tokens(std::string inputText, std::string operators) {
	std::vector<std::string> out;
	auto matches = get_all_regex_matches(inputText, "(\")?(?(1)[^\"]+\"|[^ \\t"+operators+"]+)|["+operators+"]");
	
	for (auto match : matches) out.push_back(match[0]);
	return out;
}

std::string bracket_binary_operators(std::string inputText, std::string operators) {
	auto operatorTokens = replace_by_char(operators, '\\', "");
	for (int op=0; op<operatorTokens.size(); op++) { 
		bool changed = true;
		while (changed) {
			changed = false;
			std::vector <std::string> tokens;
			auto matches = get_all_regex_matches(inputText, "(\\((?:(?1)|[^\\(\\)])*\\))|(?:(\")?(?(2)[^\"]+\"|[^ \\t="+operators+"]+)|[="+operators+"])");
			for (auto match : matches) tokens.push_back(match[0]);

			std::string out = "";
			for (int i=0; i<tokens.size(); i++) {
				auto token = tokens[i];
				if (tokens.size() != 3 && token == std::string(1, operatorTokens[op]) && tokens[i-1][tokens[i-1].size()-1] != '}') {
					changed = true;
					tokens[i-1] = "{" + tokens[i-1];
					tokens[i+1] = tokens[i+1] + "}";
				}
			}
			for (auto token : tokens) out += token + " ";

			// Use {, } and then replace with (, ) in order to avoid not parenthesising expressions such as (a + b) + (c + d) because of the first char = '}' check
			inputText = replace_by_char(replace_by_char(out, '{', "("), '}', ")");
		}
	}

	return inputText;
}

void Expression::init() {
	m_text = bracket_binary_operators(m_text, "*\\/+\\-");
	// std::cout << ":::" << bracket_binary_operators(m_text, "*\\/+\\-") << "\n";
	auto subexpressions = get_subexpressions_text(m_text);
	m_text = tokenize_subexpressions(m_text);
	m_text = compress_spaces(m_text);
	std::cout << ">>>" << m_text << "\n";

	for (auto subexpression : subexpressions) m_subexpressions.push_back(new Expression(subexpression, m_scope));

	auto tokens = split_tokens(m_text, "=*\\/+\\-");
	for (auto token : tokens) m_tokens.push_back(create_token(token, this));
	
	for (int i=0; i<m_tokens.size(); i++) {
		if (i != 0) m_tokens[i]->set_prev_token(m_tokens[i-1]);
		else m_tokens[i]->set_prev_token(nullptr);

		if (i != m_tokens.size()-1) m_tokens[i]->set_next_token(m_tokens[i+1]);
		else m_tokens[i]->set_next_token(nullptr);
	}

	std::sort(m_tokens.begin(), m_tokens.end(), [](Token* a, Token* b){return a->get_token_type() < b->get_token_type();});
	// for (auto token : m_tokens) std::cout << token->get_text() << "\n";

	// std::cout << ">> \"" << m_text << "\"\n";
	// for (auto token : m_tokens) {
	// 	std::cout << ":: \"" << token.get_text() << "\" ";
		// token.eval();
		// std::cout << "\n";
	// }
}

TokenReturn Expression::eval() {
	// for(auto token : m_tokens) token->eval();
	return m_tokens[0]->eval();
}

std::string Expression::get_text() {
	return m_text;
}

std::vector<Expression*> Expression::get_subexpressions() {
	return m_subexpressions;
}

Scope* Expression::get_scope() {
	return m_scope;
}


} // namespace stone_lang