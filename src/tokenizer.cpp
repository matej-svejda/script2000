#include "tokenizer.h"

#include <iostream>


Tokenizer::Tokenizer(const std::string& filename)
	: file_(filename)
{
	dfas_.push_back(dfaForFixedString("print", "PRINT"));
	SimpleDFA variable_name_dfa;
	variable_name_dfa.tokenName = "VARIABLE";
	variable_name_dfa.acceptingStates.insert(0);
	variable_name_dfa.transitions.push_back([] (char c)
	{
		if (std::isalnum(c))
		{
			return 0;
		}
		if (c == '_')
		{
			return 0;
		}
		return -1;
	});
	dfas_.push_back(std::move(variable_name_dfa));

	SimpleDFA number;
	number.tokenName = "NUMBER";
	number.acceptingStates.insert(0);
	number.acceptingStates.insert(1);
	number.transitions.push_back([] (char c)
	{
		if (std::isdigit(c))
		{
			return 0;
		}
		if (c == '.')
		{
			return 1;
		}
		return -1;
	});
	number.transitions.push_back([] (char c)
	{
		if (std::isdigit(c))
		{
			return 1;
		}
		return -1;
	});
	dfas_.push_back(std::move(number));
}

Tokenizer::~Tokenizer()
{
	file_.close();
}

bool Tokenizer::getNextToken(std::string& token_name, std::string& value)
{
	value = "";
	if (!file_.is_open())
	{
		return false;
	}
	auto finish_dfas = [this, &value, &token_name] ()
	{
		if (value.empty())
		{
			return false;
		}
		bool found_finished_dfa = false;
		for (SimpleDFA& dfa : dfas_)
		{
			if (!found_finished_dfa && dfa.acceptingStates.find(dfa.state) != dfa.acceptingStates.end())
			{
				token_name = dfa.tokenName;
				found_finished_dfa = true;
			}
			dfa.state = 0;
		}
		return found_finished_dfa;
	};

	char c;
	for (;;)
	{
		if (!file_.get(c))
		{
			file_.close();
			return finish_dfas();
		}

		//std::cout << "char is [" << c << "], int val " << static_cast<int>(c) << std::endl;
		if (isWhitespace(c))
		{
			if (finish_dfas())
			{
				return true;
			}
			continue;
		}
		token_name = parseSeperator(c);
		if (!token_name.empty()) // found seperator token
		{
			if (finish_dfas())
			{
				file_.unget();
			}
			return true;
		}
		for (SimpleDFA& dfa : dfas_)
		{
			if (dfa.state == -1)
			{
				continue;
			}
			dfa.state = dfa.transitions[dfa.state](c);
			//std::cout << "dfa " << dfa.tokenName << " in state " << dfa.state << std::endl;
		}
		value += c;
	}
}


Tokenizer::SimpleDFA Tokenizer::dfaForFixedString(const std::string& s, const std::string& token_name)
{
	SimpleDFA result;
	result.tokenName = token_name;
	int next_state = 1;
	for (char required_c : s)
	{
		result.transitions.push_back([required_c, next_state] (char c) { return c == required_c ? next_state : -1; });
		++next_state;
	}
	result.transitions.push_back([] (char) { return -1; });
	result.acceptingStates.insert(s.length());
	return result;
}

bool Tokenizer::isWhitespace(char c)
{
	switch (static_cast<int>(c))
	{
	case 9: // tab
		return true;
	case 10: // line feed
		return true;
	case 11: // line tabulation
		return true;
	case 12: // form feed
		return true;
	case 13: // carriage return
		return true;
	case 32: // space
		return true;
	default:
		return false;
	}
}

std::string Tokenizer::parseSeperator(char c)
{
	switch (c)
	{
	case '=':
		return "ASSIGN";
	case '+':
		return "ADD";
	case '-':
		return "SUB";
	case '*':
		return "MUL";
	case '/':
		return "DIV";
	case ';':
		return "SEMICOLON";
	case '(':
		return "OPEN_BRACKET";
	case ')':
		return "CLOSE_BRACKET";
	default:
		return "";
	}
}
