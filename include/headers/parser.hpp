#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>
#include <functional>

namespace stone_lang {

std::string read_file(std::string filePath);
std::string filter_line_after_substring(std::string inputText, std::string s);
std::string filter_by_char(std::string inputText, char c);
std::string replace_by_char(std::string inputText, char c, std::string r);
std::vector<std::vector<std::string>> get_all_regex_matches(std::string inputText, std::string regexExpression);
std::string replace_regex(std::string inputText, std::string regexExpression, std::function<std::string()> replace);
std::string replace_regex(std::string inputText, std::string regexExpression, std::string replace);
bool is_regex_match(std::string inputText, std::string regexExpression);
std::vector<std::string> get_expressions_from_text(std::string inputText);
void exec_program(std::string filePath);

} // namespace stone_lang

#endif