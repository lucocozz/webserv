/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 01:06:41 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/25 10:03:47 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <map>

typedef unsigned int	uint;

typedef enum	TokenType
{
	Keyword,	//Keyword: a-zA-Z0-9_./
	BlockStart,	//BlockStart: {
	BlockEnd,	//BlockEnd: }
	Semicolon	//Semicolon: ;
}				TokenType;

class	Token
{
public:
	TokenType	type;
	std::string	literal;
	uint		line;

	Token(TokenType type, const std::string &literal, uint line)
		: type(type), literal(literal), line(line) {}
	
	~Token() {}
};

class Lexer
{
protected:
	std::vector<Token>			_tokens;

public:

	Lexer(void) {}

	Lexer(const Lexer &src)
	{
		*this = src;
	}

	~Lexer() {}

	Lexer &operator=(const Lexer &rhs)
	{
		if (this != &rhs)
			this->_tokens = rhs._tokens;
		return (*this);
	}

	void lex(std::ifstream &file)
	{
		std::string	line;

		if (file.is_open() == false)
			throw (std::runtime_error("File not open"));
		for (uint lineNumber = 1; file.eof() == false; ++lineNumber)
		{
			std::getline(file, line);
			this->_tokenize(line, lineNumber);
		}
	}

private:
	void	_tokenize(const std::string &line, const uint &lineNumber)
	{
		std::string	literal;
		uint		column = 0;
		
		while (column < line.size())
		{
			if (line[column] == '#')
				break;
			else if (line[column] == '{')
				this->_addToken(Token(BlockStart, "{", lineNumber), column);
			else if (line[column] == '}')
				this->_addToken(Token(BlockEnd, "}", lineNumber), column);
			else if (line[column] == ';')
				this->_addToken(Token(Semicolon, ";", lineNumber), column);
			else if (this->_isKeyword(line[column]) == true)
			{
				literal = this->_getKeyword(line, column);
				this->_tokens.push_back(Token(Keyword, literal, lineNumber));
			}
			else
				++column;
		}
	}

	void	_addToken(const Token &token, uint &column)
	{
		this->_tokens.push_back(token);
		++column;
	}

	std::string	_getKeyword(const std::string &line, uint &column)
	{
		std::string	keyword;

		while (column < line.size() && this->_isKeyword(line[column]) == true)
		{
			keyword += line[column];
			++column;
		}
		return (keyword);
	}

	bool	_isKeyword(const char &c)
	{
		std::string	charset = "_./$-";

		if (std::isalnum(c) > 0 || charset.find(c) != std::string::npos)
			return (true);
		return (false);
	}
};
