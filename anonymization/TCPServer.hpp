//===-- TCPServer.hpp - A server for a sockets tcp application.-*- C++ ------*-===//
//
//                          Francisco Aguilera
//
//===----------------------------------------------------------------------------===//
///
/// \file
/// This file contains the TCPServer definition for a sockets-based tcp system.
/// A TCPServer listens for connections on a randomly given port, using the first
/// non-loopback address it can find on your local machine. Once a TCPClient connects,
/// the TCPServer and client may begin exchanging messages. The TCPServer gets second
/// priority in the exchange of messages.
///
//===----------------------------------------------------------------------------===//

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <string>       // string
#include <sys/types.h>  // socket...
#include <sys/socket.h> // socket...
#include <netdb.h>      // socket...
#include <arpa/inet.h>  // ntohs...
#include <unistd.h>     // _SC_HOST_NAME_MAX, sysconf, gethostname, close

#include "EAIException.hpp"
#include "SystemException.hpp"
#include "TCPSystem.hpp"

class TCPServer : public TCPSystem
{
public:
	static const int backlog = 1;

	TCPServer()
	: TCPServer(0)
	{

	}

	TCPServer(uint16_t port)
	: serverSocketFD_(0),
	  serverPort_(port)
	{
		serverHost_.resize(sysconf(_SC_HOST_NAME_MAX));
		if (-1 == gethostname(&serverHost_[0], serverHost_.size()))
		{
			throw SystemException();
		}

		struct addrinfo hints
		{
			AI_PASSIVE, PF_UNSPEC, SOCK_STREAM
		}, * results, * result;
		void *ipAddr;
		int enable = 1;

		int errorCode = getaddrinfo(&serverHost_[0], "0", &hints, &results);
		if (errorCode != 0)
		{
			throw EAIException(errorCode);
		}

		for (result = results; result != nullptr; result = result->ai_next)
		{
			if ((serverSocketFD_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == -1)
			{
				continue;
			}

			if (setsockopt(serverSocketFD_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
			{
				throw SystemException();
			}

			struct sockaddr * saddr = (struct sockaddr *)result->ai_addr;
			if(0 != serverPort_)
			{
				if (AF_INET == result->ai_family)
				{
					((struct sockaddr_in *)saddr)->sin_port = htons(serverPort_);
				}
				else if (AF_INET6 == result->ai_family)
				{
					((struct sockaddr_in6 *)saddr)->sin6_port = htons(serverPort_);
				}
				else
				{
					throw SystemException("Error: Could not set port.");
				}
			}

			if (bind(serverSocketFD_, saddr, result->ai_addrlen) == -1)
			{
				close(serverSocketFD_);
				continue;
			}

			struct sockaddr updatedAddr, * portAddr;
			socklen_t updatedAddrLen = sizeof(updatedAddr);
			if (getsockname(serverSocketFD_, (struct sockaddr *)&updatedAddr, &updatedAddrLen) == -1)
			{
				throw SystemException();
			}
			portAddr = &updatedAddr;

			if (AF_INET == result->ai_family)
			{
				ipAddr = &((struct sockaddr_in *)result->ai_addr)->sin_addr;
				serverIp_.resize(INET_ADDRSTRLEN);
				serverPort_ = ntohs(((struct sockaddr_in *)portAddr)->sin_port);
			}
			else if (AF_INET6 == result->ai_family)
			{
				ipAddr = &((struct sockaddr_in6 *)result->ai_addr)->sin6_addr;
				serverIp_.resize(INET6_ADDRSTRLEN);
				serverPort_ = ntohs(((struct sockaddr_in6 *)portAddr)->sin6_port);
			}
			else
			{
				throw SystemException("Error: Unknown interface address family.");
			}
			inet_ntop(result->ai_family, ipAddr, &serverIp_[0], serverIp_.size());

			break;
		}

		freeaddrinfo(results);

		if (result == nullptr)
		{
			throw SystemException();
		}
	}

	~TCPServer()
	{
		if (0 != serverSocketFD_)
		{
			close(serverSocketFD_);
		}
		if (0 != clientSocketFD_)
		{
			close(clientSocketFD_);
		}
	}

	void listen()
	{
		if (::listen(serverSocketFD_, backlog) == -1)
		{
			throw SystemException();
		}

		struct sockaddr_storage clientAddr;
		socklen_t clientAddrSize = sizeof(clientAddr);
		if ((clientSocketFD_ = ::accept(serverSocketFD_, (struct sockaddr *)&clientAddr, &clientAddrSize)) == -1)
		{
			throw SystemException();
		}
	}

	const std::string getHostname() const
	{
		return serverHost_;
	}

	const std::string getIp() const
	{
		return serverIp_;
	}

	const std::string getPort() const
	{
		return std::to_string(serverPort_);
	}

private:
	int serverSocketFD_;
	std::string serverHost_;
	std::string serverIp_;
	uint16_t serverPort_;
};

#endif