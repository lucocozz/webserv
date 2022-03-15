/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 01:11:36 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/14 17:18:51 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <vector>
#include <string>
# include "Parser.hpp"

#ifndef DEFAULT_PORT
# define DEFAULT_PORT "8080"
#endif

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
	std::vector<ServerContext>	servers;

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
		this->_checkConfig();
	}



private:
	void	_checkConfig(void)
	{
		this->_checkPortValidity();
	}

	void	_checkPortValidity(void)
	{
		std::pair<std::vector<std::string>, std::vector<std::string> >					directives;
		std::vector<std::pair<std::vector<std::string>, std::vector<std::string> > >	ports;

		for (size_t i = 0; i < this->servers.size(); ++i)
		{
			this->servers[i].directives["listen"] = this->_resolveListen(this->servers[i].directives["listen"]);
			directives.first = this->servers[i].directives["listen"];
			directives.second = this->servers[i].directives["server_name"];
			if (std::find(ports.begin(), ports.end(), directives) != ports.end())
				throw (std::runtime_error("Duplicate port: " + directives.first[0] + ":" + directives.first[1]));
			ports.push_back(directives);
		}
	}

	std::vector<std::string>	_resolveListen(std::vector<std::string> &listen)
	{
		std::vector<std::string>	resolve;

		if (listen.size() > 2)
			throw (std::runtime_error("Invalid listen directive"));
		if (listen.size() == 0)
		{
			resolve.push_back("0.0.0.0");
			resolve.push_back(DEFAULT_PORT);
		}
		else if (listen[0].find(".") != std::string::npos)
		{
			resolve.push_back(listen[0]);
			resolve.push_back(DEFAULT_PORT);
		}
		else
		{
			resolve.push_back("0.0.0.0");
			resolve.push_back(listen[0]);
		}
		return (resolve);
	}

	void	_parsingToData(void)
	{
		for (size_t i = 0; i < this->_parsed.size(); ++i)
			this->servers.push_back(this->_parsedServerToData(this->_parsed[i].block));
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
