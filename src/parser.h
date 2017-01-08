#pragma once

#include <stack>

#include "parse_table.h"

class Parser
{
public:
	Parser(const std::vector<std::vector<std::string>>& rules, const std::vector<std::vector<ParseTableEntry>>& parse_table);

	void readToken(const std::string& token);

private:
	ParseTableEntry entryForSymbol(const std::string& token);

	std::vector<std::vector<ParseTableEntry>> parse_table_;
	std::vector<std::vector<std::string>> rules_;
	std::stack<int> stack_;
};
