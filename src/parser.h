#pragma once

#include <stack>
#include <memory>
#include <vector>

#include "parse_table_calculation.h"

struct ASTNode
{
	std::string token;
	std::string value;
	int ruleIndex; // -1 means its a terminal
	std::vector<ASTNode> children;

	ASTNode() {}

	ASTNode(const std::string& token, const std::string& value, int rule_index)
		: token(token), value(value), ruleIndex(rule_index) {}

};

class Parser
{
public:
	Parser(const std::vector<std::vector<std::string>>& rules, const std::vector<std::vector<ParseTableEntry>>& parse_table);

	bool read(const std::string& token, const std::string& value, bool print_debug = false);
	const ASTNode& getASTRoot();
	void printAST();

private:
	bool entryForToken(const std::string& token, ParseTableEntry& entry, bool print_debug);

	std::vector<std::vector<ParseTableEntry>> parse_table_;
	std::vector<std::vector<std::string>> rules_;
	std::stack<int> stack_;
	std::stack<ASTNode> tree_nodes_stack_;
};
