#pragma once

#include <vector>
#include <string>
#include <initializer_list>

enum ParseAction
{
	ParseActionShift,
	ParseActionReduce,
	ParseActionGoto,
	ParseActionAccept
};

struct ParseTableEntry
{
	ParseAction action;
	std::string symbol;
	int target;

	ParseTableEntry(ParseAction action, const std::string& symbol, int target)
		: action(action), symbol(symbol), target(target) {}
};

std::vector<std::vector<ParseTableEntry>> calculateParseTable(const std::vector<std::vector<std::string>>& rules);
