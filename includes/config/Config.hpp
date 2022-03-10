/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 01:11:36 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/10 14:31:18 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <vector>
#include <string>
# include "Parser.hpp"

struct LocationContext
{
	std::vector<std::string>							args;
	std::map<std::string, std::vector<std::string> >	directives;
};

struct ServerContext
{
	std::map<std::string, std::vector<std::string> >	directives;
	std::vector<LocationContext>						locations;
};

class Config: public Lexer, public Parser
{
private:
	std::string					_filePath;

public:
	ServerContext	server;

	Config(void): Lexer(), Parser() {}

	Config(const Config &src): Lexer(), Parser()
	{
		*this = src;
	}

	~Config() {}

	Config	&operator=(const Config &rhs)
	{
		if (this != &rhs)
		{
			this->Lexer::operator=(rhs);
			this->Parser::operator=(rhs);
			this->_filePath = rhs._filePath;
		}
		return (*this);
	}

	void	parse(const std::string &filePath)
	{
		std::ifstream	fileStream;
		
		this->_filePath = filePath;
		fileStream.open(filePath.c_str());
		if (fileStream.is_open() == false)
			throw (std::runtime_error("Can't open file: " + filePath));
		this->Lexer::lex(fileStream);
		fileStream.close();
		this->Parser::parse(this->_tokens);
		this->_parsingToData();
	}



private:
	void	_parsingToData(void)
	{
		for (size_t i = 0; i < this->_parsed.size(); ++i)
			this->server = this->_parsedServerToData(this->_parsed[i].block);
	}

	ServerContext	_parsedServerToData(const std::vector<Directive> &block)
	{
		ServerContext			server;

		for (size_t i = 0; i < block.size(); ++i)
		{
			if (block[i].literal == "location")
				server.locations.push_back(this->_parsedLocationToData(block[i].block));
			else
				server.directives[block[i].literal] = block[i].args;
		}
		return (server);
	}

	LocationContext	_parsedLocationToData(const std::vector<Directive> &block)
	{
		LocationContext			location;

		for (size_t i = 0; i < block.size(); ++i)
		{
			location.args = block[i].args;
			location.directives[block[i].literal] = block[i].args;
		}
		return (location);
	}
};
