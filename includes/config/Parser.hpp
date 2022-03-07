/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 01:09:40 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/07 15:49:57 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <utility>
#include <sstream>
#include "Lexer.hpp"

#ifndef WEBSERV_PATH
# define WEBSERV_PATH NULL
#endif

typedef struct Directive
{
	std::string					directive;
	uint						line;
	std::vector<std::string>	args;
	std::vector<Directive>		block;

} Directive;




class Parser
{
private:
	std::vector<std::string>	_directivesServer;
	std::vector<std::string>	_directivesLocation;

protected:
	std::vector<Directive>		_parsed;

public:
	Parser(void)
	{
		this->_readDirectives("server.txt", this->_directivesServer);
		this->_readDirectives("location.txt", this->_directivesLocation);
	}

	Parser(const Parser &src)
	{
		*this = src;
	}

	~Parser() {}

	Parser &operator=(const Parser &rhs)
	{
		if (this != &rhs)
		{
			this->_directivesServer = rhs._directivesServer;
			this->_directivesLocation = rhs._directivesLocation;
			this->_parsed = rhs._parsed;
		}
		return (*this);
	}

	void	parse(std::vector<Token> &tokens)
	{
		size_t	i = 0;

		this->_checkBrackets(tokens);
		this->_parsed = this->_parseDirective(tokens, i);
		this->_checkParsing();
	}

private:
	void	_checkParsing(void)
	{
	}

	std::vector<Directive>	_parseDirective(std::vector<Token> &tokens, size_t &i)
	{
		Directive				directive;
		std::vector<Directive>	directiveList;

		for (; i < tokens.size(); ++i)
		{
			directive = Directive();
			if (tokens[i].type == BlockEnd)
				return (directiveList);
			if (tokens[i].type == Keyword)
				directive.directive = tokens[i].literal;
			else
				throw (std::runtime_error(this->_errorMsg(tokens[i].line, "Invalid directive: " + tokens[i].literal)));
			directive.line = tokens[i].line;
			for (++i ; i < tokens.size() && tokens[i].type == Keyword; ++i)
				directive.args.push_back(tokens[i].literal);
			if (tokens[i].type == BlockStart)
				directive.block = this->_parseDirective(tokens, ++i);
			else if (tokens[i].type != Semicolon)
				throw (std::runtime_error(this->_errorMsg(tokens[i].line, "Missing ';'")));
			directiveList.push_back(directive);
		}
		return (directiveList);
	}

	void	_checkBrackets(std::vector<Token> &tokens)
	{
		int	depth = 0;

		for (size_t i = 0; i < tokens.size(); ++i)
		{
			if (tokens[i].type == BlockStart)
				++depth;
			else if (tokens[i].type == BlockEnd)
				--depth;
			if (depth < 0)
				throw (std::runtime_error(this->_errorMsg(tokens[i].line, "unexpected '}'")));
		}
		if (depth > 0)
			throw (std::runtime_error("unexpected end of file, missing '}'"));
	}

	void	_readDirectives(const std::string &file, std::vector<std::string> &directives)
	{
		std::string		line;
		std::ifstream	directivesFile;
		std::string		path(WEBSERV_PATH);

		path += ("config/directives/" + file);
		directivesFile.open(path.c_str());
		if (directivesFile.is_open() == false)
			throw (std::runtime_error("Can't open file: " + path));
		while (directivesFile.eof() == false)
		{
			std::getline(directivesFile, line);
			if (line.empty() == false)
				directives.push_back(line);
		}
	}

	std::string	_errorMsg(const uint line, const std::string message)
	{
		std::stringstream	buffer;

		buffer << line << ": " << message;
		return (buffer.str());
	}
};
