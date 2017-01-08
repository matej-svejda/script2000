#include "parse_table.h"

#include <map>
#include <set>
#include <list>
#include <cassert>
#include <iostream>

struct Item
{
	int ruleIndex;
	int dotPosition;

	Item(int ruleIndex, int dotPosition)
		: ruleIndex(ruleIndex), dotPosition(dotPosition)
	{
		assert(dotPosition > 0);
	}

	bool operator< (const Item& i) const
	{
		if (ruleIndex == i.ruleIndex)
		{
			return dotPosition < i.dotPosition;
		}
		return ruleIndex < i.ruleIndex;
	}

	bool operator== (const Item& i) const
	{
		return ruleIndex == i.ruleIndex && dotPosition == i.dotPosition;
	}
};

std::string rightOfDot(const Item& item, const std::vector<std::vector<std::string>>& rules)
{
	if (item.dotPosition >= static_cast<int>(rules[item.ruleIndex].size()))
	{
		return "";
	}
	return rules[item.ruleIndex][item.dotPosition];
}

void closeItemSet(const std::vector<std::vector<std::string>>& rules, const std::map<std::string, std::vector<int>>& non_terminal_to_rule_indices, std::set<Item>& set)
{
	std::vector<Item> items(set.begin(), set.end());
	for (;;)
	{
		std::vector<Item> added_items;
		for (const Item& item : items)
		{
			std::string right_of_dot = rightOfDot(item, rules);
			if (right_of_dot.empty())
			{
				continue;
			}
			auto found = non_terminal_to_rule_indices.find(right_of_dot);
			if (found == non_terminal_to_rule_indices.end())
			{
				continue;
			}
			for (int rule_index : found->second)
			{
				assert(rule_index < static_cast<int>(rules.size()));
				Item item(rule_index, 1);
				if (set.find(item) == set.end())
				{
					set.insert(item);
					added_items.push_back(item);
				}
			}
		}
		if (added_items.empty())
		{
			return;
		}
		items = added_items;
	}
}

void printItemSet(const std::set<Item>& set, const std::vector<std::vector<std::string>>& rules)
{
	for (const Item& item : set)
	{
		for (int i = 0; i < static_cast<int>(rules[item.ruleIndex].size()); ++i)
		{
			if (i == item.dotPosition)
			{
				std::cout << ".";
			}
			std::cout << rules[item.ruleIndex][i] << " ";
		}
		if (item.dotPosition == static_cast<int>(rules[item.ruleIndex].size()))
		{
			std::cout << ".";
		}
		std::cout << std::endl;
	}
}


void printTable(const std::vector<std::vector<ParseTableEntry>>& table, const std::list<std::set<Item>>& item_sets, const std::vector<std::vector<std::string>>& rules)
{
	int i = 0;
	for (auto& set : item_sets)
	{
		std::cout << "State [" << i << "]\n";
		printItemSet(set, rules);
		std::cout << std::endl;
		++i;
	}

	i = 0;
	for (const auto& row : table)
	{
		std::cout << i << " ";
		for (const ParseTableEntry& entry : row)
		{
			std::cout << "\"" << entry.symbol << "\",";
			switch (entry.action)
			{
			case ParseActionAccept:
				std::cout << "accept,";
				break;
			case ParseActionGoto:
				std::cout << "goto,";
				break;
			case ParseActionReduce:
				std::cout << "reduce,";
				break;
			case ParseActionShift:
				std::cout << "shift,";
				break;
			}
			std::cout << entry.target << " ";
		}
		std::cout << std::endl;
		++i;
	}
}

std::vector<std::vector<ParseTableEntry>> calculateParseTable(const std::vector<std::vector<std::string>>& rules)
{
	assert(rules.front().size() == 3);
	assert(rules.front()[2] == "eof");

	std::map<std::string, std::vector<int>> non_terminal_to_rule_indices;
	for (int i = 0; i < static_cast<int>(rules.size()); ++i)
	{
		non_terminal_to_rule_indices[rules[i].front()].push_back(i);
	}

	std::list<std::set<Item>> item_sets;
	item_sets.push_back({{0, 1}});
	closeItemSet(rules, non_terminal_to_rule_indices, item_sets.front());

	std::vector<std::vector<ParseTableEntry>> result;
	for (auto& set : item_sets)
	{
		result.push_back({});
		std::set<std::string> handled_transitions;
		for (const Item& item : set)
		{
			std::string right_of_dot = rightOfDot(item, rules);
			if (right_of_dot.empty())
			{
				// We only reduce
				result.back() = {ParseTableEntry(ParseActionReduce, "", item.ruleIndex)};
				break;
			}
			if (handled_transitions.find(right_of_dot) != handled_transitions.end())
			{
				continue;
			}
			if (right_of_dot == "eof")
			{
				result.back().push_back(ParseTableEntry(ParseActionAccept, "eof", -1));
				continue;
			}
			handled_transitions.insert(right_of_dot);
			std::set<Item> new_set;
			for (const Item& item : set)
			{
				if (rightOfDot(item, rules) == right_of_dot)
				{
					new_set.insert({item.ruleIndex, item.dotPosition + 1});
				}
			}
			closeItemSet(rules, non_terminal_to_rule_indices, new_set);
			int j = 0;
			for (const auto set : item_sets)
			{
				if (new_set == set)
				{
					break;
				}
				++j;
			}

			if (j == static_cast<int>(item_sets.size()))
			{
				item_sets.push_back(new_set);
			}
			ParseAction parse_action;
			if (std::isupper(right_of_dot[0]))
			{
				parse_action = ParseActionGoto;
			}
			else
			{
				parse_action = ParseActionShift;
			}
			result.back().push_back(ParseTableEntry(parse_action, right_of_dot, j));
		}
	}

	printTable(result, item_sets, rules);

	return result;
}
