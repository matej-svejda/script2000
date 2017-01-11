#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>
#include <list>


enum ParseAction
{
	ParseActionShift,
	ParseActionReduce,
	ParseActionAccept
};

struct ParseTableEntry
{
	ParseAction action;
	int target;
	std::string token;

	ParseTableEntry() {}

	ParseTableEntry(ParseAction action, int target, const std::string& token)
		: action(action), target(target), token(token) {}

	bool operator== (const ParseTableEntry& i) const
	{
		return action == i.action && target == i.target && token == i.token;
	}
};


class ParseTableCalculation
{
public:
	ParseTableCalculation(const std::vector<std::vector<std::string>>& rules);

	std::vector<std::vector<ParseTableEntry>> calculateTable();
	void printTable(const std::vector<std::vector<ParseTableEntry>>& table);

	static bool isTerminal(const std::string& token);

private:
	struct Item
	{
		int ruleIndex;
		int dotPosition;
		std::string lookahead;

		Item(int rule_index, int dot_position, const std::string& lookahead);
		bool operator< (const Item& i) const;
		bool operator== (const Item& i) const;
	};


	std::string rightOfDot(const Item& item);
	std::set<std::string> getFirstSet(const std::vector<std::string>& tokens);
	void closeItemSet(std::set<Item>& set);
	void printItemSets(const std::list<std::set<Item>>& item_sets);
	std::vector<std::vector<ParseTableEntry>> applyOperatorPrecedence(std::vector<std::vector<ParseTableEntry>>& table);

	std::vector<std::vector<std::string>> rules_;
	std::map<std::string, std::vector<int>> non_terminal_to_rule_indices_;
	std::map<std::string, std::set<std::string>> non_terminal_first_sets_;
};
