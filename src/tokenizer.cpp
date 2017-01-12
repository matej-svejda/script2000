#include "tokenizer.h"


Tokenizer::Tokenizer(const std::string& filename)
	: file_(filename)
{

}

Tokenizer::~Tokenizer()
{
	file_.close();
}

bool Tokenizer::getNextToken(std::string& token_name, std::string& value)
{
	char c;
	if (!file_.get(c))
	{
		return false;
	}


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
	case 30: // space
		return true;
	default:
		return false;
	}
}

std::string Tokenizer::parseSeperator(char c)
{
	switch (c)
	{
	case '+':
		return "PLUS";
	case '-':
		return "MINUS";
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
