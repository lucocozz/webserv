/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 01:09:40 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/25 09:58:51 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <set>
#include <string>
#include <utility>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "Lexer.hpp"

#ifndef WEBSERV_PATH
# define WEBSERV_PATH NULL
#endif

typedef struct Directive
{
	std::string					literal;
	uint						line;
	std::vector<std::string>	args;
	std::vector<Directive>		block;
} Directive;




class Parser
{
private:
	std::set<std::string>	_directivesServer;
	std::set<std::string>	_directivesLocation;

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
		// this->_directiveInclude(this->_parsed);
		this->_checkParsing();
	}

private:
	// void	_directiveInclude(std::vector<Directive> &directives)
	// {
	// 	std::vector<Directive>	include;

	// 	for (size_t i = 0; i < directives.size(); ++i)
	// 	{
	// 		if (directives[i].block.size() > 0)
	// 			this->_directiveInclude(directives[i].block);
	// 		if (directives[i].literal == "include")
	// 		{
	// 			include = this->_parseInclude(directives[i].args[0]);
	// 			directives.insert(directives.begin() + i + 1, include.begin(), include.end());
	// 		}
	// 	}
	// }

	// std::vector<Directive>	_parseInclude(std::string path)
	// {
	// 	(void)path;
	// }

	void	_checkParsing(void)
	{
		for (size_t i = 0; i < this->_parsed.size(); ++i)
		{
			if (this->_parsed[i].literal == "server")
				this->_checkServerBlock(this->_parsed[i].block);
			else
				throw (std::runtime_error(this->_errorMsg(this->_parsed[i].line,
					"Unknow directive: " + this->_parsed[i].literal)));
		}
	}

	void	_checkServerBlock(std::vector<Directive> &block)
	{
		for (size_t i = 0; i < block.size(); ++i)
		{
			if (block[i].literal == "location")
				this->_checkLocationBlock(block[i].block);
			else if (this->_directivesServer.find(block[i].literal) == this->_directivesServer.end())
				throw (std::runtime_error(this->_errorMsg(block[i].line,
					"Unknow directive: " + block[i].literal)));
		}
	}

	void	_checkLocationBlock(std::vector<Directive> &block)
	{
		for (size_t i = 0; i < block.size(); ++i)
		{
			if (this->_directivesLocation.find(block[i].literal) == this->_directivesLocation.end())
				throw (std::runtime_error(this->_errorMsg(block[i].line,
					"Unknow directive: " + block[i].literal)));
		}
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
				directive.literal = tokens[i].literal;
			else
				throw (std::runtime_error(this->_errorMsg(tokens[i].line, "Invalid directive: " + tokens[i].literal)));
			directive.line = tokens[i].line;
			for (++i ; i < tokens.size() && tokens[i].type == Keyword; ++i)
				directive.args.push_back(tokens[i].literal);
			if (tokens[i].type == BlockStart)
				directive.block = this->_parseDirective(tokens, ++i);
			else if (tokens[i].type != Semicolon)
				throw (std::runtime_error(this->_errorMsg(tokens[i].line, "Don't end with ';'")));
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

	void	_readDirectives(const std::string &file, std::set<std::string> &directives)
	{
		std::string		line;
		std::ifstream	directivesFile;
		std::string		path(WEBSERV_PATH);

		path += ("directives/" + file);
		directivesFile.open(path.c_str());
		if (directivesFile.is_open() == false)
			throw (std::runtime_error("Can't open file: " + path));
		while (directivesFile.eof() == false)
		{
			std::getline(directivesFile, line);
			if (line.empty() == false)
				directives.insert(line);
		}
	}

	std::string	_errorMsg(const uint line, const std::string message)
	{
		std::stringstream	buffer;

		buffer << line << ": " << message;
		return (buffer.str());
	}
};
