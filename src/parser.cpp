#include "parser.h"

#include <iostream>
#include <cassert>

Parser::Parser(const std::vector<std::vector<std::string>>& rules, const std::vector<std::vector<ParseTableEntry>>& parse_table)
	: parse_table_(parse_table),
	  rules_(rules)
{
	stack_.push(0);
}

ParseTableEntry Parser::entryForToken(const std::string& token)
{
	auto row = parse_table_[stack_.top()];
	for (ParseTableEntry& entry : row)
	{
		if (entry.token == token)
		{
			return entry;
		}
	}
	assert(false && "Parse error");
}

void Parser::read(const std::string& token)
{
	ParseTableEntry entry;
	for (;;)
	{
		entry = entryForToken(token);
		if (entry.action != ParseActionReduce)
		{
			break;
		}
		const auto& rule = rules_[entry.target];
		std::cout << "reduce with rule " << entry.target << std::endl;
		for (int i = 0; i < static_cast<int>(rule.size() - 1); ++i)
		{
			stack_.pop();
		}
		stack_.push(entryForToken(rule.front()).target);
	}

	switch (entry.action)
	{
	case ParseActionAccept:
		std::cout << "accept" << std::endl;
		return;
	case ParseActionShift:
		std::cout << "shift state " << entry.target << std::endl;
		stack_.push(entry.target);
		return;
	}
}
