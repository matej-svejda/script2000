#pragma once

#include <stack>
#include <memory>
#include <vector>

#include "parse_table_calculation.h"

struct ASTNode
{
	std::string token;
	std::string readString;
	int ruleIndex; // -1 means its a terminal
	std::vector<ASTNode> children;

	ASTNode() {}

	ASTNode(const std::string& token, const std::string& value, int rule_index)
		: token(token), readString(value), ruleIndex(rule_index) {}

};

class Parser
{
public:
	Parser(const std::vector<std::vector<std::string>>& rules, const std::vector<std::vector<ParseTableEntry>>& parse_table, bool print_debug = false);

	bool read(const std::string& token, const std::string& read_string);
	const ASTNode& getASTRoot();
	void printAST();

private:
	bool entryForToken(const std::string& token, ParseTableEntry& entry);

	bool print_debug_;
	std::vector<std::vector<ParseTableEntry>> parse_table_;
	std::vector<std::vector<std::string>> rules_;
	std::stack<int> stack_;
	std::stack<ASTNode> tree_nodes_stack_;
};
