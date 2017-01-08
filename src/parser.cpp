#include "parser.h"

#include <iostream>
#include <cassert>

Parser::Parser(const std::vector<std::vector<std::string>>& rules, const std::vector<std::vector<ParseTableEntry>>& parse_table)
	: parse_table_(parse_table),
	  rules_(rules)
{
	stack_.push(0);
}

ParseTableEntry Parser::entryForSymbol(const std::string& symbol)
{
	auto row = parse_table_[stack_.top()];
	for (ParseTableEntry& entry : row)
	{
		std::cout << entry.target << " " << entry.action << std::endl;
		if (entry.symbol == symbol || entry.action == ParseActionReduce)
		{
			return entry;
		}
	}
	assert(false);
}

void Parser::readToken(const std::string& token)
{
	for (;;)
	{
		ParseTableEntry entry = entryForSymbol(token);
		switch (entry.action)
		{
		case ParseActionAccept:
		{
			std::cout << "accept" << std::endl;
			return;
		}
		case ParseActionGoto:
		{
			assert(false);
			break;
		}
		case ParseActionReduce:
		{
			const auto& rule = rules_[entry.target];
			std::cout << "reduce with rule " << entry.target << std::endl;
			for (int i = 0; i < static_cast<int>(rule.size() - 1); ++i)
			{
				stack_.pop();
			}
			stack_.push(entryForSymbol(rule.front()).target);
			break;
		}
		case ParseActionShift:
		{
			std::cout << "shift state " << entry.target << std::endl;
			stack_.push(entry.target);
			return;
		}
		}
	}

}
