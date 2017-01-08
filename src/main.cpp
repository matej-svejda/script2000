#include <iostream>

#include "parse_table.h"
#include "parser.h"

int main()
{
	/*std::vector<std::vector<std::string>> grammar = {
		{"S'", "S", "eof"},
		{"S", "A", "A"},
		{"A", "a", "A"},
		{"A", "b"}
	};*/
	std::vector<std::vector<std::string>> rules = {
		{"S", "E", "eof"},
		{"E", "E", "mul", "B"},
		{"E", "E", "add", "B"},
		{"E", "B"},
		{"B", "zero"},
		{"B", "one"}
	};
	auto table = calculateParseTable(rules);
	Parser parser(rules, table);
	parser.readToken("one");
	parser.readToken("add");
	parser.readToken("one");
	parser.readToken("eof");
	return 0;
}

