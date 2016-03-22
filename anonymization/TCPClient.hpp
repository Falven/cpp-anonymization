//===-- TCPClient.hpp - A client for a sockets tcp application.-*- C++ --*-===//
//
//                          Francisco Aguilera
//
//===------------------------------------------------------------------------===//
///
/// \file
/// This file contains the TCPClient definition for a sockets-based tcp system.
/// A TCPClient can connect to a TCPServer to exchange messages.
/// The TCPClient requires the IP and port of the TCPServer to communicate.
/// The TCPClient is the first to exchange a message with a TCPServer.
///
//===------------------------------------------------------------------------===//

#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <string>       // string
#include <sys/types.h>  // socket...
#include <sys/socket.h> // socket...
#include <netdb.h>      // socket...
#include <unistd.h>     // close

#include "EAIException.hpp"
#include "SystemException.hpp"
#include "TCPSystem.hpp"

class TCPClient : public TCPSystem
{
public:
	TCPClient(std::string port, std::string ip)
	{

		struct addrinfo hints
		{
			AI_PASSIVE, PF_UNSPEC, SOCK_STREAM
		}, *results, *result;

		int errorCode = getaddrinfo(ip.c_str(), port.c_str(), &hints, &results);
		if (errorCode != 0)
		{
			throw EAIException(errorCode);
		}

		for (result = results; result != nullptr; result = result->ai_next)
		{
			if ((clientSocketFD_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == -1)
			{
				continue;
			}
			break;
		}

		if (-1 == ::connect(clientSocketFD_, result->ai_addr, result->ai_addrlen))
		{
			throw SystemException();
		}

		freeaddrinfo(results);

		if (result == nullptr)
		{
			throw SystemException();
		}
	}

	~TCPClient()
	{
		if (0 != clientSocketFD_)
		{
			close(clientSocketFD_);
		}
	}
};

#endif