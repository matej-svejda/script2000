#include <iostream>

#include "parse_table_calculation.h"
#include "parser.h"

int main()
{
	/*std::vector<std::vector<std::string>> grammar = {
		{"S'", "S", "EOF"},
		{"S", "A", "A"},
		{"A", "a", "A"},
		{"A", "b"}
	};*/
	/*std::vector<std::vector<std::string>> rules = {
		{"S", "E", "EOF"},
		{"E", "E", "mul", "B"},
		{"E", "E", "add", "B"},
		{"E", "B"},
		{"B", "zero"},
		{"B", "one"}
	};*/

	/*
program ::= commands.
commands ::= command SEMICOLON commands.
commands ::= .
command ::= PRINT rvalue. { t->print(); }
command ::= VARIABLE(var) ASSIGN rvalue. { t->assign(var); }
rvalue ::= rvalue SUB rvalue. { t->sub(); }
rvalue ::= rvalue ADD rvalue. { t->add(); }
rvalue ::= rvalue MUL rvalue. { t->mul(); }
rvalue ::= rvalue DIV rvalue. { t->div(); }
rvalue ::= NUMBER(num). { t->pushNumber(num); }
rvalue ::= VARIABLE(var). { t->pushVariable(var); }
	*/

	std::vector<std::vector<std::string>> rules = {
		{"program", "commands", "EOF"},
		{"commands", "command", "SEMICOLON", "commands"},
		{"commands", ""},
		{"command", "PRINT", "rvalue"},
		{"command", "VARIABLE", "ASSIGN", "rvalue"},
		{"rvalue", "rvalue", "SUB", "rvalue"},
		{"rvalue", "rvalue", "ADD", "rvalue"},
		{"rvalue", "rvalue", "MUL", "rvalue"},
		{"rvalue", "rvalue", "DIV", "rvalue"},
		{"rvalue", "OPEN_BRACKET", "rvalue", "CLOSE_BRACKET"},
		{"rvalue", "NUMBER"},
		{"rvalue", "VARIABLE"}
	};
	ParseTableCalculation table_calculation(rules);
	auto table = table_calculation.calculateTable();
	table_calculation.printTable(table);
	Parser parser(rules, table);
	std::vector<std::pair<std::string, std::string>> tokens = {

		{"VARIABLE", "a"},
		{"ASSIGN", ""},
		{"NUMBER", "0.5"},
		{"MUL", ""},
		{"NUMBER", "100"},
		{"DIV", ""},
		{"OPEN_BRACKET", ""},
		{"NUMBER", "1"},
		{"ADD", ""},
		{"NUMBER", "2"},
		{"CLOSE_BRACKET", ""},
		{"MUL", ""},
		{"NUMBER", "3"},
		{"SEMICOLON", ""},

		{"VARIABLE", "b"},
		{"ASSIGN", ""},
		{"VARIABLE", "a"},
		{"SUB", ""},
		{"NUMBER", "4"},
		{"SEMICOLON", ""},

		{"PRINT", ""},
		{"VARIABLE", "a"},
		{"SEMICOLON", ""},

		{"PRINT", ""},
		{"VARIABLE", "b"},
		{"SEMICOLON", ""},

		{"EOF", ""}
	};
	for (const auto& token : tokens)
	{
		bool success = parser.read(token.first, token.second, true);
		if (!success)
		{
			return 1;
		}
	}
	parser.printAST();
	return 0;
}

