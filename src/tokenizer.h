#pragma once

#include <string>
#include <fstream>

class Tokenizer
{
public:
	Tokenizer(const std::string& filename);
	~Tokenizer();

	bool getNextToken(std::string& token_name, std::string& value);

private:
	bool isWhitespace(char c);
	std::string parseSeperator(char c);

	std::ifstream file_;

};
