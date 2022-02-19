/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 22:47:17 by lucocozz          #+#    #+#             */
/*   Updated: 2022/02/19 19:54:34 by lucocozz         ###   ########.fr       */
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
	int					_listenSocket;

public:
	Socket(int fd = 0): _listenSocket(fd) {}

	Socket(const Socket &rhs): _listenSocket(0)
	{
		*this = rhs;
	}

	~Socket() {}

	Socket	&operator=(const Socket &rhs)
	{
		if (this != &rhs)
			this->_listenSocket = rhs._listenSocket;
		return (*this);
	}

	int	listener(void) const
	{
		return (this->_listenSocket);
	}

	void	createSocket(int socktype = SOCK_STREAM)
	{
		if (this->_listenSocket != 0)
			throw (std::runtime_error("Socket already created"));
		std::cout << "Creating socket..." << std::endl;
		this->_listenSocket = socket(AF_INET, socktype, 0);
		if (this->_listenSocket == -1)
			throw (std::runtime_error(strerror(errno)));
		std::cout << "New socket as: " << this->_listenSocket << std::endl;
	}

	void	bindSocket(uint16_t port, int listenAddress = INADDR_ANY)
	{
		struct sockaddr_in	bindAddress;
		
		if (this->_listenSocket == 0)
			throw (std::runtime_error("Socket have not been created yet"));
		std::cout << "Binding socket to address..." << std::endl;
		bzero(&bindAddress, sizeof(bindAddress));
		bindAddress.sin_family = AF_INET;
		bindAddress.sin_port = htons(port);
		bindAddress.sin_addr.s_addr = htonl(listenAddress);
		if (bind(this->_listenSocket, (struct sockaddr*)&bindAddress, sizeof(bindAddress)) == -1)
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
		std::cout << "Client is connected as socket: " << socketClient << std::endl;
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

	void	shutdownSocket(int how = SHUT_RDWR)
	{
		if (shutdown(this->_listenSocket, how) == -1)
			throw (std::runtime_error(strerror(errno)));
	}

	void	closeSocket(void)
	{
		std::cout << "Closing socket: " << this->_listenSocket << std::endl;
		if (close(this->_listenSocket) == -1)
			throw (std::runtime_error(strerror(errno)));
		this->_listenSocket = 0;
	}

	void	setNonBlocking(void)
	{
		fcntl(this->_listenSocket, F_SETFL, O_NONBLOCK);
	}
};