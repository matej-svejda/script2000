#include "parse_table_calculation.h"

#include <cassert>
#include <iostream>


ParseTableCalculation::ParseTableCalculation(const std::vector<std::vector<std::string>>& rules)
	: rules_(rules)
{
	assert(rules_.front().size() == 3);
	assert(rules_.front()[2] == "eof");

	for (int i = 0; i < static_cast<int>(rules_.size()); ++i)
	{
		non_terminal_to_rule_indices_[rules_[i].front()].push_back(i);
	}
}

bool isTerminal(const std::string& token)
{
	return std::islower(token[0]);
}

std::set<std::string> ParseTableCalculation::getFirstSet(const std::vector<std::string>& tokens)
{
	if (tokens.empty())
	{
		return {};
	}
	const std::string& token = tokens.front();
	if (token.empty())
	{
		return {""};
	}
	if (isTerminal(token))
	{
		return {token};
	}
	if (non_terminal_first_sets_.find(token) == non_terminal_first_sets_.end())
	{
		for (int rule_index : non_terminal_to_rule_indices_[token])
		{
			const auto& rule = rules_[rule_index];
			auto recursive_result = getFirstSet(std::vector<std::string>(rule.begin() + 1, rule.end()));
			non_terminal_first_sets_[token].insert(recursive_result.begin(), recursive_result.end());
		}
	}
	auto set = non_terminal_first_sets_[token];
	if (set.find("") == set.end())
	{
		return set;
	}
	auto recursive_result = getFirstSet(std::vector<std::string>(tokens.begin() + 1, tokens.end()));
	recursive_result.insert(set.begin(), set.end());
	return recursive_result;
}

std::string ParseTableCalculation::rightOfDot(const Item& item)
{
	if (item.dotPosition >= static_cast<int>(rules_[item.ruleIndex].size()))
	{
		return "";
	}
	return rules_[item.ruleIndex][item.dotPosition];
}

void ParseTableCalculation::closeItemSet(std::set<Item>& set)
{
	std::vector<Item> items(set.begin(), set.end());
	for (;;)
	{
		std::vector<Item> added_items;
		for (const Item& item : items)
		{
			std::string right_of_dot = rightOfDot(item);
			if (right_of_dot.empty() || isTerminal(right_of_dot))
			{
				continue;
			}
			auto found = non_terminal_to_rule_indices_.find(right_of_dot);
			assert(found != non_terminal_to_rule_indices_.end());
			const auto& rule = rules_[item.ruleIndex];
			std::vector<std::string> after_non_terminal_tokens(rule.begin() + item.dotPosition + 1, rule.end());
			after_non_terminal_tokens.push_back(item.lookahead);
			auto first_set = getFirstSet(after_non_terminal_tokens);
			for (int rule_index : found->second)
			{
				assert(rule_index < static_cast<int>(rules_.size()));
				for (const auto& token : first_set)
				{
					Item item(rule_index, 1, token);
					if (set.find(item) == set.end())
					{
						set.insert(item);
						added_items.push_back(item);
					}
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

void ParseTableCalculation::printItemSet(const std::set<Item>& set)
{
	for (const Item& item : set)
	{
		for (int i = 0; i < static_cast<int>(rules_[item.ruleIndex].size()); ++i)
		{
			if (i == item.dotPosition)
			{
				std::cout << ".";
			}
			std::cout << rules_[item.ruleIndex][i] << " ";
		}
		if (item.dotPosition == static_cast<int>(rules_[item.ruleIndex].size()))
		{
			std::cout << ".";
		}
		std::cout << " | " << item.lookahead << std::endl;
	}
}


void ParseTableCalculation::printTable(const std::vector<std::vector<ParseTableEntry>>& table, const std::list<std::set<Item>>& item_sets)
{
	int i = 0;
	for (auto& set : item_sets)
	{
		std::cout << "State [" << i << "]\n";
		printItemSet(set);
		std::cout << std::endl;
		++i;
	}

	i = 0;
	for (const auto& row : table)
	{
		std::cout << i << " ";
		for (const ParseTableEntry& entry : row)
		{
			std::cout << "\"" << entry.token << "\",";
			switch (entry.action)
			{
			case ParseActionAccept:
				std::cout << "accept,";
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

std::vector<std::vector<ParseTableEntry>> ParseTableCalculation::calculateTable()
{
	std::list<std::set<Item>> item_sets;
	item_sets.push_back({Item(0, 1, "eof")});
	closeItemSet(item_sets.front());

	std::vector<std::vector<ParseTableEntry>> result;
	for (auto& set : item_sets)
	{
		result.push_back({});
		std::set<std::string> handled_transitions;
		for (const Item& item : set)
		{
			std::string right_of_dot = rightOfDot(item);
			if (right_of_dot.empty())
			{
				result.back().push_back(ParseTableEntry(ParseActionReduce, item.ruleIndex, item.lookahead));
				continue;
			}
			if (handled_transitions.find(right_of_dot) != handled_transitions.end())
			{
				continue;
			}
			if (right_of_dot == "eof")
			{
				result.back().push_back(ParseTableEntry(ParseActionAccept, -1, "eof"));
				continue;
			}
			handled_transitions.insert(right_of_dot);
			std::set<Item> new_set;
			for (const Item& item : set)
			{
				if (rightOfDot(item) == right_of_dot)
				{
					new_set.insert(Item(item.ruleIndex, item.dotPosition + 1, item.lookahead));
				}
			}
			closeItemSet(new_set);
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
			result.back().push_back(ParseTableEntry(ParseActionShift, j, right_of_dot));
		}
	}

	printTable(result, item_sets);

	return result;
}



ParseTableCalculation::Item::Item(int rule_index, int dot_position, const std::string& lookahead)
	: ruleIndex(rule_index), dotPosition(dot_position), lookahead(lookahead)
{
	assert(dot_position > 0);
}

bool ParseTableCalculation::Item::operator< (const Item& i) const
{
	if (ruleIndex == i.ruleIndex)
	{
		if (dotPosition == i.dotPosition)
		{
			return lookahead < i.lookahead;
		}
		return dotPosition < i.dotPosition;
	}
	return ruleIndex < i.ruleIndex;
}

bool ParseTableCalculation::Item::operator== (const Item& i) const
{
	return ruleIndex == i.ruleIndex && dotPosition == i.dotPosition && lookahead == i.lookahead;
}
