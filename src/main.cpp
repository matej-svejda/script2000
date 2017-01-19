#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <sstream>

#include "parse_table_calculation.h"
#include "parser.h"
#include "tokenizer.h"

std::unordered_map<std::string, std::string> parseArguments(const std::string default_key, int argc, char* argv[])
{
	std::unordered_map<std::string, std::string> parsed;
	std::string last_parsed_key;
	for (int i = 1; i < argc; ++i)
	{
		std::string argument = argv[i];
		if (argument.empty())
		{
			continue;
		}
		if (argument.size() >= 2 && argument.substr(0, 2) == "--")
		{
			last_parsed_key = argument.substr(2);
			parsed[last_parsed_key] = "";
		}
		else
		{
			if (!last_parsed_key.empty())
			{
				parsed[last_parsed_key] = argument;
			}
			else
			{
				parsed[default_key] = argument;
			}
		}
	}
	return parsed;
}

void printError(const std::string& text)
{
	std::cout << "\033[1;31m" << text << "\033[0m" << std::endl;
}

void printHelp()
{
	std::cout << std::endl << "script2000 interpreter ᕙ༼ຈل͜ຈ༽ᕗ" << std::endl << std::endl
			  << "Usage:" << std::endl
			  << "\tscript2000 [--filename] filname [--table_debug] [--parser_debug] [--ast] [--debug]" << std::endl << std::endl;
}

int main(int argc, char* argv[])
{
	auto args = parseArguments("filename", argc, argv);

	if (args.find("help") != args.end())
	{
		printHelp();
		return 0;
	}

	auto it_filename = args.find("filename");
	if (it_filename == args.end())
	{
		std::cout << std::endl;
		printError("No file provided");
		printHelp();
		return -1;
	}

	bool print_table_calculation_debug = args.find("table_debug") != args.end();
	bool print_parser_debug = args.find("parser_debug") != args.end();
	if (args.find("debug") != args.end())
	{
		print_table_calculation_debug = true;
		print_parser_debug = true;
	}
	bool print_ast = args.find("ast") != args.end();
	std::string filename = it_filename->second;

	std::vector<std::vector<std::string>> rules = {
		{"program", "commands", "EOF"},
		{"commands", "command", "SEMICOLON", "commands"},
		{"commands", "block", "commands"},
		{"commands", ""},
		{"command", "PRINT", "rvalue"},
		{"command", "VARIABLE", "ASSIGN", "rvalue"},
		{"command", "function_call"},
		{"command", "return"},
		{"block", "OPEN_CURLY_BRACKET", "commands", "CLOSE_CURLY_BRACKET"},
		{"function", "FUNCTION", "parameter_list", "block"},
		{"parameter_list", "OPEN_BRACKET", "parameters", "CLOSE_BRACKET"},
		{"parameter_list", "OPEN_BRACKET", "CLOSE_BRACKET"},
		{"parameters", "parameter", "COMMA", "parameters"},
		{"parameters", "parameter"},
		{"parameter", "VARIABLE"},
		{"function_call", "VARIABLE", "parameter_list"},
		{"return", "RETURN", "rvalue"},
		{"rvalue", "rvalue", "SUB", "rvalue"},
		{"rvalue", "rvalue", "ADD", "rvalue"},
		{"rvalue", "rvalue", "MUL", "rvalue"},
		{"rvalue", "rvalue", "DIV", "rvalue"},
		{"rvalue", "OPEN_BRACKET", "rvalue", "CLOSE_BRACKET"},
		{"rvalue", "STRING"},
		{"rvalue", "NUMBER"},
		{"rvalue", "VARIABLE"},
		{"rvalue", "function"}

	};
	ParseTableCalculation table_calculation(rules, print_table_calculation_debug);
	auto table = table_calculation.calculateTable();
	Parser parser(rules, table, print_parser_debug);

	Tokenizer tokenizer(filename);
	std::string token, read_string;
	while (token != "EOF")
	{
		bool success = tokenizer.getNextToken(token, read_string);
		if (success)
		{
			success = parser.read(token, read_string);
		}
		if (!success)
		{
			std::string line = std::to_string(tokenizer.getCurrentLine());
			printError("An error occurred on line " + line + " when parsing [" + read_string + "]");
			return 1;
		}
	}
	if (print_ast)
	{
		parser.printAST();
	}
	return 0;
}

