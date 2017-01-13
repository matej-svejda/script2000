#pragma once

#include <string>
#include <fstream>
#include <functional>
#include <vector>
#include <set>

class Tokenizer
{
public:
	Tokenizer(const std::string& filename);
	~Tokenizer();

	bool getNextToken(std::string& token_name, std::string& value);

private:
	struct SimpleDFA
	{
		std::string tokenName;
		int state = 0;
		std::set<int> acceptingStates;
		std::vector<std::function<int(char)>> transitions;
	};

	static bool isWhitespace(char c);
	static std::string parseSeperator(char c);
	static SimpleDFA dfaForFixedString(const std::string& s, const std::string& token_name);

	std::ifstream file_;
	std::vector<SimpleDFA> dfas_;
};
