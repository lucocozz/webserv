/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 22:47:17 by lucocozz          #+#    #+#             */
/*   Updated: 2022/02/17 17:13:16 by lucocozz         ###   ########.fr       */
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

#define REQUEST_BUFFER 1024

#include <sstream>

template<typename T>
std::string toString(const T &value)
{
    std::ostringstream oss;

    oss << value;
    return (oss.str());
}

class Socket
{
protected:
	int				_listenSocket;
	struct addrinfo	*_bindAddress;

private:
	std::string	_getnameinfo(int socketFd, int flags = 0)
	{
		socklen_t				hostLen;
		struct sockaddr_storage	hostAddress;
		char					addressBuffer[128];

		hostLen = sizeof(hostAddress);
		getpeername(socketFd, (struct sockaddr*)&hostAddress, &hostLen);
		getnameinfo((struct sockaddr*)&hostAddress, hostLen, addressBuffer,
			sizeof(addressBuffer), NULL, 0, flags);
		return (addressBuffer);
	}

public:
	Socket(): _listenSocket(0), _bindAddress(NULL) {}

	Socket(int fd): _listenSocket(fd), _bindAddress(NULL) {}

	Socket(int family, int socktype, int flags, std::string port)
	{
		struct addrinfo	hints;

		bzero(&hints, sizeof(hints));
		hints.ai_family = family;
		hints.ai_socktype = socktype;
		hints.ai_flags = flags;
		getaddrinfo(NULL, port.c_str(), &hints, &this->_bindAddress);
		std::cout << "Creating socket..." << std::endl;
		this->_listenSocket = socket(this->_bindAddress->ai_family,
			this->_bindAddress->ai_socktype, this->_bindAddress->ai_protocol);
		if (this->_listenSocket == -1)
			throw (std::runtime_error(strerror(errno)));
	}

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

	struct addrinfo	*addrInfo(void) const
	{
		return (this->_bindAddress);
	}

	void	bindSocket(void)
	{
		std::cout << "Binding socket to local address..." << std::endl;
		if (bind(this->listener(), this->_bindAddress->ai_addr, this->_bindAddress->ai_addrlen) == -1)
			throw (std::runtime_error(strerror(errno)));
	}

	void	listenSocket(int backlog = 10)
	{
		std::cout << "Listening..." << std::endl;
		if (listen(this->_listenSocket, backlog) == -1)
			throw (std::runtime_error(strerror(errno)));
	}

	Socket	acceptConnection(void)
	{
		int						socketClient;
		socklen_t				clientLen;
		struct sockaddr_storage	clientAddress;

		clientLen = sizeof(clientAddress);
		socketClient = accept(this->_listenSocket, (struct sockaddr*)&clientAddress, &clientLen);
		if (socketClient == -1)
			throw (std::runtime_error(strerror(errno)));
		std::cout << "Client is connected... " << this->_getnameinfo(socketClient) << std::endl;
		return (socketClient);
	}

	std::string	recvData(Socket &target, int flags = 0)
	{
		char	request[REQUEST_BUFFER] = {0};
		int		bytesReceived = 0;

		std::cout << "Reading data..." << std::endl;
		bytesReceived = recv(target.listener(), request, REQUEST_BUFFER, flags);
		if (bytesReceived == -1)
			throw (std::runtime_error(strerror(errno)));
		std::cout << "Received " + toString(bytesReceived) + " bytes." << std::endl;
		return (request);
	}

	void	sendData(Socket &target, std::string data, int flags = 0)
	{
		int	bytesSent;

		std::cout << "Sending data..." <<std::endl;
		bytesSent = send(target.listener(), data.c_str(), data.length(), flags);
		if (bytesSent == -1)
			throw (std::runtime_error(strerror(errno)));
		std::cout << "Sent " << toString(bytesSent) << " of " << toString(data.length()) << " bytes." << std::endl;
	}

	void	shutdownSocket(int how = SHUT_RDWR)
	{
		if (shutdown(this->_listenSocket, how) == -1)
			throw (std::runtime_error(strerror(errno)));
	}

	void	closeSocket(void)
	{
		close(this->_listenSocket);
	}
};