/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/03 01:11:36 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/07 15:27:50 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <string>
# include "Parser.hpp"

class Config: public Lexer, public Parser
{
private:
	std::string					_filePath;

public:
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
		this->Parser::parse(this->_tokens);
		fileStream.close();
	}
};
