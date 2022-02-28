/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/21 17:45:09 by lucocozz          #+#    #+#             */
/*   Updated: 2022/02/28 11:46:02 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <list>
#include <map>

typedef unsigned int	uint;

typedef enum	TokenType
{
	Keyword,			//Keyword: a-zA-Z0-9_./
	BlockStart = '{',	//BlockStart: {
	BlockEnd = '}',		//BlockEnd: }
	Semicolon = ';'		//Semicolon: ;
}				TokenType;

class	Token
{
public:
	TokenType	type;
	std::string	literal;
	uint		column;
	uint		line;

	Token(TokenType type, std::string literal, uint column, uint line)
		: type(type), literal(literal), column(column), line(line) {}
	
	~Token() {}
};

class Lexer
{
private:
	std::list<Token>			_tokenList;
	std::map<char, std::string>	_tokenType;

public:
	typedef typename std::list<Token>::iterator	iterator;




	Lexer(void)
	{
		this->_tokenType[BlockStart] = "{";
		this->_tokenType[BlockEnd] = "}";
		this->_tokenType[Semicolon] = ";";
	}

	~Lexer() {}




	iterator	begin(void)
	{
		return (this->_tokenList.begin());
	}

	iterator	end(void)
	{
		return (this->_tokenList.end());
	}




	void	tokenize(const std::string &line, const uint &lineNumber)
	{
		uint		i;
		std::string	literal;
		uint		column = 0;
		
		while (column < line.size())
		{
			if (line[column] == '#')
				break;
			else if (std::iswspace(line[column]))
				++column;
			else if (this->_tokenType.find(line[column]) != this->_tokenType.end())
			{
				this->_tokenList.push_back(Token(TokenType(line[column]), this->_tokenType[line[column]], column, lineNumber));
				++column;
			}
			else if (this->_isKeyword(line[column]) == true)
			{
				for (i = 0; this->_isKeyword(line[column + i]) == true; ++i)
					literal += line[column + i];
				this->_tokenList.push_back(Token(Keyword, literal, column, lineNumber));
				column += i;
				literal.clear();
			}
			else
				throw (std::runtime_error("Lexer: invalid character"));
		}
	}




private:
	bool	_isKeyword(char c)
	{
		if (std::isalnum(c) > 0 || c == '_' || c == '/' || c == '.')
			return (true);
		return (false);
	}
};




class Parser
{
private:
	Lexer			_lexer;
	std::string		_fileName;
	std::ifstream	_parsedFile;

public:
	Parser(void) {}

	Parser(const std::string &fileName)
	{
		this->open(fileName);
	}

	~Parser()
	{
		this->close();
	}




	void open(const std::string &fileName)
	{
		this->_fileName = fileName;
		if (this->_parsedFile.is_open() == true)
			throw (std::runtime_error("File already open"));
		this->_parsedFile.open(fileName.c_str());
		if (this->_parsedFile.is_open() == false)
			throw (std::runtime_error("Can't open file:" + fileName));
	}

	void close(void)
	{
		if (this->_parsedFile.is_open() == true)
			this->_parsedFile.close();
	}




	void lexing(void)
	{
		std::string	line;

		for (uint lineNumber = 0; this->_parsedFile.eof() == false; ++lineNumber)
		{
			std::getline(this->_parsedFile, line);
			this->_lexer.tokenize(line, lineNumber);
		}
	}
};

int	main(void)
{
	Parser	parser;

	try {
		parser.open("default.conf");
		parser.lexing();
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
