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
	enum CodeModeStates
	{
		WaitingForInput = 0,
		ReadingVariable,
		ReadingNumber,
		ReadingNumberAfterDot,
		CodeModeStatesCount
	};

	typedef std::function<int(char c, const std::string& read_string, std::string& token_name)> StateTransition;

	struct SimpleDFA
	{
		std::string tokenName;
		int state = 0;
		std::vector<StateTransition> transitions;
		StateTransition globalTransition;
		std::unordered_set<int> acceptingStates;
	};

	bool moveAcrossWhitespace();
	void finishDFAs(std::string& token_name);
	void transitionDFAs(char c);

	static bool detectWhitespace(char c);
	static bool detectSeperator(char c, std::string& token_name);
	static SimpleDFA dfaForFixedString(const std::string& s, const std::string& token_name);

	int lines_count_ = 0;
	std::ifstream file_;
	std::vector<SimpleDFA> dfas_;

};
