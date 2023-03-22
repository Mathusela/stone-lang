#include "headers/parser.hpp"

#include <fstream>
#include <sstream>
#include <functional>
#include <vector>
#include <boost/regex.hpp>

#include <iostream>

#include "headers/expression.hpp"

namespace stone_lang {

std::string read_file(std::string filePath) {
	std::ifstream fs(filePath);

	if (!fs) throw std::exception(("Failed to load file " + filePath + ".").c_str());

	std::string text;
	std::string line;
	while (std::getline(fs, line)) text.append(line + "\n");

	return text;
}

std::string filter_by_line(std::string inputText, std::function<bool(std::string)> func) {
	std::istringstream ss(inputText);

	std::string text;
	std::string line;
	while (std::getline(ss, line)) if (func(line)) {text.append(line + "\n");}

	return text;
}

int max(int x, int y) {
	return x > y ? x : y;
}

std::string filter_line_after_substring(std::string inputText, std::string s) {
	std::istringstream ss(inputText);

	std::string text;
	std::string line;
	std::string lineOut = "";
	while (std::getline(ss, line)) {
		lineOut = "";
		bool stop = false;
		// std::cout << line.size() << " " << s.size();
		for (int i=0; i<((int)line.size()-(int)s.size()); i++) {
			if (line.substr(i, s.size()) == s) stop = true;
			if (!stop) lineOut += line[i];
		}
		if (!stop) for (int i=max(0, (int)line.size()-(int)s.size()); i<line.size(); i++) lineOut += line[i];
		text += lineOut + "\n";
	}

	return text;
}

std::string filter_by_char(std::string inputText, char c) {
	std::string text = "";

	for (int i=0; i<inputText.size(); i++) if (inputText[i] != c) text.append(std::string(1, inputText[i]));

	return text;
}

std::string replace_by_char(std::string inputText, char c, std::string r) {
	std::string text = "";

	for (int i=0; i<inputText.size(); i++) {
		if (inputText[i] == c) text.append(r);
		else text.append(std::string(1, inputText[i]));
	}

	return text;
}

std::string remove_whitespace(std::string inputText) {
	return filter_by_char(inputText, '\t');
}

std::string remove_comments(std::string inputText) {
	return filter_line_after_substring(inputText, "//");
}

std::string remove_newlines(std::string inputText) {
	return filter_by_char(inputText, '\n');
}

std::string escape_reserved_chars(std::string inputText) {
	return replace_by_char(replace_by_char(replace_by_char(inputText, '\\', "\\\\"), '$', "\\$"), '%', "\\%");
}

std::string initial_format_code(std::string inputText) {
	return escape_reserved_chars(remove_newlines(remove_comments(remove_whitespace(inputText))));
}

std::vector<unsigned int> find_all(std::string inputText, char c) {
	std::vector<unsigned int> out {};
	
	for (unsigned int i=0; i<inputText.size(); i++) if (inputText[i] == c) out.push_back(i);
	
	return out;
}

std::vector<std::vector<std::string>> get_all_regex_matches(std::string inputText, std::string regexExpression) {
	std::vector<std::vector<std::string>> out {};
		
	boost::regex r (regexExpression);
	
	boost::smatch sm;
	while(boost::regex_search(inputText, sm, r)) {
		std::vector<std::string> groups;
		for (auto group : sm) groups.push_back(group);
		out.push_back(groups);
		inputText = sm.suffix();
	}

	return out;
}

std::string replace_regex(std::string inputText, std::string regexExpression, std::function<std::string()> replace) {
	boost::regex r (regexExpression);
	while (boost::regex_search(inputText, r))
		inputText = boost::regex_replace(inputText, r, replace(), boost::format_first_only);
	return inputText;
}


std::vector<std::string> get_scopes(std::string inputText) {
	std::vector<std::string> out {};

	auto matches = get_all_regex_matches(inputText, "{((?:(?R)|[^{}])*)}");
	for (auto match : matches) out.push_back(match[1]);

	return out;
}

std::string tokenize_scopes(std::string inputText) {
	return replace_regex(inputText, "{((?:(?R)|[^{}])*)}", [](){static int count=0; return "\\$"+std::to_string(count++)+"\\$;";} );
}

std::vector<std::string> get_expressions(std::string inputText) {
	std::vector<std::string> out;

	std::string expression = "";
	for (auto c : inputText) {
		if (c != ';') expression += c;
		else {
			out.push_back(expression);
			expression = "";
		}
	}

	return out;
}

std::string tokenize_subexpressions(std::string inputText) {
	return replace_regex(inputText, "\\(((?:(?R)|[^\\(\\)}])*)\\)", [](){static int count=0; return "%"+std::to_string(count++)+"%";} );
}

std::vector<std::string> get_subexpressions(std::string inputText) {
	std::vector<std::string> out {};

	auto matches = get_all_regex_matches(inputText, "\\(((?:(?R)|[^\\(\\)}])*)\\)");
	for (auto match : matches) out.push_back(match[1]);

	return out;
}

void parse_text(std::string inputText) {
	auto formattedText = initial_format_code(inputText);
	auto scopes = get_scopes(formattedText);
	auto scopeTokensText = tokenize_scopes(formattedText);
	auto expressions = get_expressions(scopeTokensText);

	std::cout << inputText << "\n\n";
	std::cout << scopeTokensText << "\n\n";
	for (auto expression : expressions) std::cout << tokenize_subexpressions(expression) << "\n";
	std::cout << "\n";
	for (auto expression : expressions) for (auto subexpression : get_subexpressions(expression)) std::cout << subexpression << "\n";
	std::cout << "\n";
}

void test_func() {
	std::string fileText = read_file("../../test-code/test.st");
	
	// stone_lang::Expression {};
	parse_text(fileText);
}

} // namespace stone_lang