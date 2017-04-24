#pragma once

#include <string>
#include <fstream>
#include <functional>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <regex>

class Tokenizer
{
public:
	Tokenizer(const std::string& filename);

	bool getNextToken(std::string& token_name, std::string& read_string);
	int getCurrentLine();

private:
	void readInWithoutComments(const std::string& filename);
	static void trim(std::string& string);

	std::vector<std::string> lines_;
	int current_line_index_ = -1;

	static constexpr char COMMENT = '#';
	static const std::vector<std::pair<std::regex, std::string>> MATCHES;
	static const std::regex WHITESPACE_REGEX;
};
