#pragma once

#include <string>
#include <fstream>
#include <functional>
#include <vector>
#include <unordered_set>
#include <algorithm>

class Tokenizer
{
public:
	Tokenizer(const std::string& filename);
	~Tokenizer();

	bool getNextToken(std::string& token_name, std::string& read_string);
	int getCurrentLine();

private:
	struct SimpleDFA
	{
		std::string tokenName;
		int state = 0;
		std::unordered_set<int> acceptingStates;
		std::vector<std::function<int(char)>> transitions;
	};

	bool moveAcrossWhitespace();
	void finishDFAs(std::string& token_name);
	void transitionDFAs(char c);

	bool readWhitespace(char c);
	static std::string parseSeperator(char c);
	static SimpleDFA dfaForFixedString(const std::string& s, const std::string& token_name);

	int lines_count_ = 0;
	std::ifstream file_;
	std::vector<SimpleDFA> dfas_;

};
