/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 22:47:17 by lucocozz          #+#    #+#             */
/*   Updated: 2022/02/23 22:55:34 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <netdb.h>
#include <netinet/in.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>

#define RECV_BUFFER 1024

class Socket
{
protected:
	int				_listenSocket;
	struct addrinfo	*_bindAddress;

public:
	Socket(int fd = 0): _listenSocket(fd), _bindAddress(NULL) {}

	Socket(const Socket &rhs): _listenSocket(0), _bindAddress(NULL)
	{
		*this = rhs;
	}

	~Socket()
	{
		if (this->_bindAddress != NULL)
			freeaddrinfo(this->_bindAddress);
	}

	Socket	&operator=(const Socket &rhs)
	{
		int	size;

		if (this != &rhs)
		{
			this->_listenSocket = rhs._listenSocket;
			if (this->_bindAddress != NULL)
				freeaddrinfo(this->_bindAddress);
			this->_bindAddress = NULL;
			if (rhs._bindAddress != NULL)
			{
				size = sizeof(rhs._bindAddress);
				this->_bindAddress = new struct addrinfo[size];
				memcpy(this->_bindAddress, rhs._bindAddress, size);
			}
		}
		return (*this);
	}





	int	listener(void) const
	{
		return (this->_listenSocket);
	}





	void	createSocket(int family, int socktype, std::string port, int flags = 0)
	{
		struct addrinfo	hints;

		if (this->_listenSocket != 0)
			throw (std::runtime_error("Socket already created"));
		std::cout << "Creating socket..." << std::endl;
		bzero(&hints, sizeof(hints));
		hints.ai_family = family;
		hints.ai_socktype = socktype;
		hints.ai_flags = flags;
		getaddrinfo(NULL, port.c_str(), &hints, &this->_bindAddress);
		this->_listenSocket = socket(this->_bindAddress->ai_family,
			this->_bindAddress->ai_socktype, this->_bindAddress->ai_protocol);
		if (this->_listenSocket == -1)
			throw (std::runtime_error(strerror(errno)));
	}

	void	setNonBlocking(void)
	{
		fcntl(this->_listenSocket, F_SETFL, O_NONBLOCK);
	}
	
	void	closeSocket(void)
	{
		std::cout << "Closing socket: " << this->getNameInfo(this->_listenSocket) << std::endl;
		if (close(this->_listenSocket) == -1)
			throw (std::runtime_error(strerror(errno)));
		this->_listenSocket = 0;
	}
	
	void	shutdownSocket(int how = SHUT_RDWR)
	{
		if (shutdown(this->_listenSocket, how) == -1)
			throw (std::runtime_error(strerror(errno)));
	}


	

	void	bindSocket(void)
	{
		if (this->_listenSocket == 0)
			throw (std::runtime_error("Socket have not been created yet"));
		std::cout << "Binding socket to address..." << std::endl;
		if (bind(this->listener(), this->_bindAddress->ai_addr, this->_bindAddress->ai_addrlen) == -1)
			throw (std::runtime_error(strerror(errno)));
	}

	void	listenSocket(int backlog = 10)
	{
		std::cout << "Listening..." << std::endl;
		if (listen(this->_listenSocket, backlog) == -1)
			throw (std::runtime_error(strerror(errno)));
	}

	int	acceptConnection(void)
	{
		int						socketClient;
		socklen_t				clientLen;
		struct sockaddr_storage	clientAddress;

		std::cout << "Incoming connection..." << std::endl;
		clientLen = sizeof(clientAddress);
		socketClient = accept(this->_listenSocket, (struct sockaddr*)&clientAddress, &clientLen);
		if (socketClient == -1)
			throw (std::runtime_error(strerror(errno)));
		std::cout << "Client is connected as: " << this->getNameInfo(socketClient) << std::endl;
		return (socketClient);
	}




	std::pair<std::string, int>	recvData(int flags = 0)
	{
		char	buffer[RECV_BUFFER] = {0};
		int		bytesReceived = 0;

		std::cout << "Reading data..." << std::endl;
		bytesReceived = recv(this->_listenSocket, buffer, RECV_BUFFER, flags);
		if (bytesReceived == -1)
			throw (std::runtime_error(strerror(errno)));
		std::cout << "Received " << bytesReceived << " bytes." << std::endl;
		return (std::make_pair(buffer, bytesReceived));
	}

	int	sendData(std::string data, int flags = 0)
	{
		int	bytesSent;

		std::cout << "Sending data..." <<std::endl;
		bytesSent = send(this->_listenSocket, data.c_str(), data.length(), flags);
		if (bytesSent == -1)
			throw (std::runtime_error(strerror(errno)));
		std::cout << "Sent " << bytesSent << " of " << data.length() << " bytes." << std::endl;
		return (bytesSent);
	}




	std::string	getNameInfo(int socketFd, int flags = 0)
	{
		socklen_t				hostLen;
		struct sockaddr_storage	hostAddress;
		char					addressBuffer[128] = {0};

		hostLen = sizeof(hostAddress);
		if (getpeername(socketFd, (struct sockaddr*)&hostAddress, &hostLen) == -1)
			std::cerr << "getpeername(): " << strerror(errno) << std::endl;
		if (getnameinfo((struct sockaddr*)&hostAddress, hostLen, addressBuffer, sizeof(addressBuffer), NULL, 0, flags) == -1)
			std::cerr << "getnameinfo(): " << strerror(errno) << std::endl;
		return (addressBuffer);
	}
};
