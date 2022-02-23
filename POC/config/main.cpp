/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/21 17:45:09 by lucocozz          #+#    #+#             */
/*   Updated: 2022/02/23 21:26:17 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

typedef enum	TokenType
{
	Keyword,	//any keyword
	BlockStart,	//BlockStart {
	BlockEnd,	//BlockEnd }
	Semicolon,	//Semicolon ;
	Comment,	//Comment #
	Illegal		//Illegal a token that should never happen
}				TokenType;

typedef struct	Token
{
	TokenType	type;
	std::string	literal;
}				Token;

int	main(void)
{
	
	return (0);
}
