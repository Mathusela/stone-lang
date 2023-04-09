#include "headers/parser.hpp"

#include <fstream>
#include <sstream>
#include <boost/regex.hpp>

#include <iostream>

#include "headers/expression.hpp"
#include "headers/scope.hpp"

namespace stone_lang {

std::string read_file(std::string filePath) {
	std::ifstream fs(filePath);

	if (!fs) throw std::exception(("Failed to load file " + filePath + ".").c_str());

	std::string text;
	std::string line;
	while (std::getline(fs, line)) text.append(line + "\n");

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

bool is_regex_match(std::string inputText, std::string regexExpression) {
	boost::regex r (regexExpression);
	return boost::regex_match(inputText, r);
}

std::string replace_regex(std::string inputText, std::string regexExpression, std::function<std::string()> replace) {
	boost::regex r (regexExpression);
	while (boost::regex_search(inputText, r))
		inputText = boost::regex_replace(inputText, r, replace(), boost::format_first_only);
	return inputText;
}
std::string replace_regex(std::string inputText, std::string regexExpression, std::string replace) {
	boost::regex r (regexExpression);
	return boost::regex_replace(inputText, r, replace);
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

std::vector<std::string> get_expressions_from_text(std::string inputText) {
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

void exec_program(std::string filePath) {
	std::string fileText = read_file(filePath);
	auto formattedText = initial_format_code(fileText);

	Scope global(formattedText, "global");
	global.eval();
}

} // namespace stone_lang