#include "tokenizer.h"

#include <iostream>

const std::vector<std::pair<std::regex, std::string>> Tokenizer::MATCHES = {
	{std::regex("^;"), "SEMICOLON"},
	{std::regex("^print"), "PRINT"},
	{std::regex("^function"), "FUNCTION"},
	{std::regex("^return"), "RETURN"},
	{std::regex("^\\{"), "OPEN_CURLY_BRACKET"},
	{std::regex("^\\}"), "CLOSE_CURLY_BRACKET"},
	{std::regex("^\\("), "OPEN_BRACKET"},
	{std::regex("^\\)"), "CLOSE_BRACKET"},
	{std::regex("^,"), "COMMA"},
	{std::regex("^="), "ASSIGN"},
	{std::regex("^-"), "SUB"},
	{std::regex("^\\+"), "ADD"},
	{std::regex("^\\*"), "MUL"},
	{std::regex("^/"), "DIV"},
	{std::regex("^\"[a-zA-Z\\s]*\""), "STRING"},
	{std::regex("^[a-zA-Z][a-zA-Z_0-9]*"), "VARIABLE"},
	{std::regex("^[0-9][0-9]*(\\.[0-9]*)?"), "NUMBER"},
};

const std::regex Tokenizer::WHITESPACE_REGEX("^ +| +$|( ) +");

Tokenizer::Tokenizer(const std::string& filename)
{
	readInWithoutComments(filename);
	current_line_index_ = 0;
}

bool Tokenizer::getNextToken(std::string& token_name, std::string& read_string)
{
	while (lines_.front().empty())
	{
		lines_.erase(lines_.begin(), lines_.begin() + 1);
		++current_line_index_;
		if (lines_.empty())
		{
			token_name = "EOF";
			return true;
		}
	}
	std::string& line = lines_.front();
	for (const auto& pair : MATCHES)
	{
		std::smatch color_match;
		if (std::regex_search(line, color_match, pair.first))
		{
			token_name = pair.second;
			read_string = color_match[0];
			line = line.substr(read_string.length());
			trim(read_string);
			trim(line);
			return true;
		}
	}
	read_string = line;
	return false;
}

int Tokenizer::getCurrentLine()
{
	return current_line_index_;
}

void Tokenizer::readInWithoutComments(const std::string& filename)
{
	std::ifstream in(filename);
	for (std::string line; std::getline(in, line);)
	{
		size_t found = line.find_first_of(COMMENT);
		if (found == std::string::npos)
		{
			lines_.push_back(line);
		}
		else
		{
			lines_.push_back(line.substr(0, found));
		}
		trim(lines_.back());
    }
	in.close();
}

void Tokenizer::trim(std::string& string)
{
	string = std::regex_replace(string, WHITESPACE_REGEX, "$1");
}
