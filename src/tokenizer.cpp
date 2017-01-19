#include "tokenizer.h"

#include <iostream>
#include <cassert>


Tokenizer::Tokenizer(const std::string& filename)
	: file_(filename)
{
	SimpleDFA code_dfa;
	code_dfa.transitions.resize(CodeModeStatesCount);
	code_dfa.transitions[WaitingForInput] = [] (char c, const std::string& read_string, std::string& token_name)
	{
		if (detectWhitespace(c))
		{
			return WaitingForInput;
		}
		if (detectSeperator(c, token_name))
		{
			// TODO write seperator token
			return WaitingForInput;
		}
		if (std::isalpha(c) || c == '_')
		{
			return ReadingVariable;
		}
		if (std::isdigit(c))
		{
			return ReadingNumber;
		}
		assert(false);
	};
	code_dfa.transitions[ReadingVariable] = [] (char c, const std::string& read_string, std::string& token_name)
	{
		std::string dummy;
		if (detectWhitespace(c) || detectSeperator(c, dummy))
		{
			if (read_string == "print")
			{
				token_name = "PRINT";
			}
			else if (read_string == "function")
			{
				token_name = "FUNCTION";
			}
			else if (read_string == "return")
			{
				token_name = "RETURN";
			}
			else
			{
				token_name = "VARIABLE";
			}
			// TODO write variable
			// TODO go back one
			return WaitingForInput;
		}
		if (std::isalnum(c) || c == '_')
		{
			return ReadingVariable;
		}
		assert(false);
	};
	code_dfa.transitions[ReadingNumber] = [] (char c, const std::string& read_string, std::string& token_name)
	{
		std::string dummy;
		if (detectWhitespace(c) || detectSeperator(c, dummy))
		{
			// write variable
			// go back one
			return WaitingForInput;
		}
		if (std::isdigit(c))
		{
			return ReadingNumber;
		}
		if (c == '.')
		{
			return ReadingNumberAfterDot;
		}
		assert(false);
	};
	code_dfa.transitions[ReadingNumberAfterDot] = [] (char c, const std::string& read_string, std::string& token_name)
	{
		std::string dummy;
		if (detectWhitespace(c) || detectSeperator(c, dummy))
		{
			// write variable
			// go back one
			return WaitingForInput;
		}
		if (std::isdigit(c))
		{
			return ReadingNumberAfterDot;
		}
		assert(false);
	};

	dfas_.push_back(dfaForFixedString("print", "PRINT"));
	dfas_.push_back(dfaForFixedString("function", "FUNCTION"));
	dfas_.push_back(dfaForFixedString("return", "RETURN"));
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


	SimpleDFA string;
	string.tokenName = "STRING";
	string.acceptingStates.insert(2);
	string.transitions.push_back([] (char c)
	{
		if (c == '"')
		{
			return 1;
		}
		return -1;
	});
	string.transitions.push_back([] (char c)
	{
		if (c == '"')
		{
			return 2;
		}
		return 1;
	});
	string.transitions.push_back([] (char c)
	{
		return -1;
	});
	dfas_.push_back(std::move(string));

}

Tokenizer::~Tokenizer()
{
	file_.close();
}

bool Tokenizer::getNextToken(std::string& token_name, std::string& read_string)
{
	read_string = "";
	if (!file_.is_open() || !moveAcrossWhitespace())
	{
		file_.close();
		token_name = "EOF";
		return true;
	}

	char c;
	for (;;)
	{
		bool finish_up = false;
		if (!file_.get(c))
		{
			file_.close();
			finish_up = true;
		}
		else if ( detectWhitespace(c))
		{
			finish_up = true;
		}
		else if (!detectSeperator(c).empty())
		{
			if (read_string.empty())
			{
				token_name = detectSeperator(c);
				read_string = c;
				return true;
			}
			file_.unget();
			finish_up = true;
		}
		if (finish_up)
		{
			if (read_string.empty())
			{
				return true;
			}
			finishDFAs(token_name);
			return !token_name.empty();
		}

		transitionDFAs(c);
		read_string += c;
	}
}


void Tokenizer::finishDFAs(std::string& token_name)
{
	token_name = "";
	for (SimpleDFA& dfa : dfas_)
	{
		if (token_name.empty() && dfa.acceptingStates.find(dfa.state) != dfa.acceptingStates.end())
		{
			token_name = dfa.tokenName;
		}
		dfa.state = 0;
	}
}

void Tokenizer::transitionDFAs(char c)
{
	for (SimpleDFA& dfa : dfas_)
	{
		if (dfa.state == -1)
		{
			continue;
		}
		dfa.state = dfa.transitions[dfa.state](c);
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

bool Tokenizer::detectWhitespace(char c)
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

bool Tokenizer::detectSeperator(char c, std::string& token_name)
{
	switch (c)
	{
	case '=':
		token_name = "ASSIGN"; return true;
	case '+':
		token_name = "ADD"; return true;
	case '-':
		token_name = "SUB"; return true;
	case '*':
		token_name = "MUL"; return true;
	case '/':
		token_name = "DIV"; return true;
	case '; return true;':
		token_name = "SEMICOLON"; return true;
	case '(':
		token_name = "OPEN_BRACKET"; return true;
	case ')':
		token_name = "CLOSE_BRACKET"; return true;
	case '{':
		token_name = "OPEN_CURLY_BRACKET"; return true;
	case '}':
		token_name = "CLOSE_CURLY_BRACKET"; return true;
	case ',':
		token_name = "COMMA"; return true;
	default:
		return false;
	}
}

bool Tokenizer::moveAcrossWhitespace()
{
	char c;
	while (file_.get(c))
	{
		if (!detectWhitespace(c))
		{
			file_.unget();
			return true;
		}
	}
	return false;
}

int Tokenizer::getCurrentLine()
{
	assert(file_.is_open());
	return lines_count_ + 1;
}
