#include "parser.h"

#include <iostream>
#include <cassert>

Parser::Parser(const std::vector<std::vector<std::string>>& rules, const std::vector<std::vector<ParseTableEntry>>& parse_table)
	: parse_table_(parse_table),
	  rules_(rules)
{
	stack_.push(0);
}

bool Parser::entryForToken(const std::string& token, ParseTableEntry& entry)
{
	auto row = parse_table_[stack_.top()];
	for (ParseTableEntry& candidate_entry : row)
	{
		if (candidate_entry.token == token)
		{
			entry = candidate_entry;
			return true;
		}
	}
	return false;
}

bool Parser::read(const std::string& token, const std::string& value, bool print_debug)
{
	ParseTableEntry entry;
	for (;;)
	{
		if (!entryForToken(token, entry))
		{
			if (print_debug)
			{
				std::cout << "An error occoured when trying to parse token [" << token << "] in state [" << stack_.top() << "]" << std::endl;
			}
			return false;
		}
		if (entry.action != ParseActionReduce)
		{
			break;
		}
		const auto& rule = rules_[entry.target];
		if (print_debug)
		{
			std::cout << "reduce with rule " << entry.target << std::endl;
		}
		ASTNode node;
		node.children.resize(rule.size() - 1);
		node.token = rule.front();
		node.ruleIndex = entry.target;
		for (int i = static_cast<int>(rule.size() - 1); i >= 1; --i)
		{
			node.children[i - 1] = tree_nodes_stack_.top();
			tree_nodes_stack_.pop();
			stack_.pop();
		}
		tree_nodes_stack_.push(node);
		ParseTableEntry new_entry;
		bool success = entryForToken(rule.front(), new_entry);
		assert(success && "Undefined state");
		stack_.push(new_entry.target);
	}

	switch (entry.action)
	{
	case ParseActionAccept:
		if (print_debug)
		{
			std::cout << "accept" << std::endl << std::endl;
		}
		break;
	case ParseActionShift:
		if (print_debug)
		{
			std::cout << "shift state " << entry.target << std::endl;
		}
		tree_nodes_stack_.push(ASTNode(token, value, -1));
		stack_.push(entry.target);
		break;
	default:
		assert(false && "Invalid action");
	}
	return true;
}


const ASTNode& Parser::getASTRoot()
{
	assert(tree_nodes_stack_.size() == 1);
	return tree_nodes_stack_.top();
}

void printASTNode(const ASTNode& node, const std::string& node_prefix, const std::string& children_prefix, const std::string& last_child_prefix)
{
	std::cout << node_prefix << node.token;
	if (!node.value.empty())
	{
		std::cout << " (" << node.value << ")";
	}
	std::cout << std::endl;
	for (int i = 0; i < static_cast<int>(node.children.size()); ++i)
	{
		const auto& child = node.children[i];
		if (i < static_cast<int>(node.children.size()) - 1)
		{
			printASTNode(child, children_prefix + "`-------", children_prefix + "|       ", children_prefix + "|       ");
		}
		else
		{
			// Last child
			printASTNode(child, last_child_prefix + "`-------", last_child_prefix + "|       ", last_child_prefix + "        ");
		}
	}
}

void Parser::printAST()
{
	std::cout << "Abstract Syntax Tree" << std::endl;
	printASTNode(getASTRoot(), "\t", "\t", "\t");
	std::cout << std::endl;
}


