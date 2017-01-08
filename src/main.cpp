#include <iostream>

#include "parse_table_calculation.h"
#include "parser.h"

int main()
{
	/*std::vector<std::vector<std::string>> grammar = {
		{"S'", "S", "eof"},
		{"S", "A", "A"},
		{"A", "a", "A"},
		{"A", "b"}
	};*/
	/*std::vector<std::vector<std::string>> rules = {
		{"S", "E", "eof"},
		{"E", "E", "mul", "B"},
		{"E", "E", "add", "B"},
		{"E", "B"},
		{"B", "zero"},
		{"B", "one"}
	};*/
	std::vector<std::vector<std::string>> rules = {
		{"S'", "S", "eof"},
		{"S", "A", "B"},
		{"A", "a", "A", "b"},
		{"A", "a"},
		{"B", "d"}
	};
	ParseTableCalculation table_calculation(rules);
	auto table = table_calculation.calculateTable();
	Parser parser(rules, table);
	std::vector<std::string> tokens = {"a", "a", "a", "b", "b", "d", "eof"};
	for (const std::string& token : tokens)
	{
		parser.read(token);
	}
	return 0;
}

