//===-- TCPSystem.hpp - A base system for a sockets tcp application.-*- C++
//--*-===//
//
//                          Francisco Aguilera
//
//===----------------------------------------------------------------------------===//
///
/// \file
/// This file contains the TCPSystem base definition for a sockets-based tcp
/// system.
/// A TCPSystem is the base class for TCPClients and TCPServers, and provides
/// the common base requirements necessary in a sockets-based TCP System;
/// the ability to send and receive messages using a socket. The TCPSystem also
/// maintains a version and packet size exchanged between TCPServers and
/// TCPClients.
///
//===----------------------------------------------------------------------------===//

#ifndef TCP_SYSTEM_H
#define TCP_SYSTEM_H

#include <string>       // string
#include <vector>       // vector
#include <iterator>     // next
#include <sys/types.h>  // send, recv
#include <sys/socket.h> // send, recv
#include <stdexcept>    // invalid_argument
#include <string.h>     // memcpy
#include <fcntl.h>		// open
#include <sys/stat.h>	// struct stat

#include "SystemException.hpp"

class TCPSystem
{
public:
	TCPSystem() : clientSocketFD_(0)
	{
	}

	void sendMessage(const std::string & message)
	{
		std::string::size_type messageSize = message.size();
		uint16_t netMessageSize = htons(messageSize);
		std::size_t headerSize = sizeof(uint16_t);
		uint16_t packetSize = headerSize + messageSize;

		std::vector<char> buffer(packetSize);
		memcpy(&(buffer[0]), &netMessageSize, headerSize);

		std::copy(message.begin(), message.end(), std::next(buffer.begin(), headerSize));
		for (ssize_t bytesSent = 0; bytesSent < packetSize;)
		{
			uint16_t sendAmount = (packetSize < MAX_MSG_SIZE) ? packetSize : MAX_MSG_SIZE;
			bytesSent += ::send(clientSocketFD_, &(buffer[bytesSent]), sendAmount, 0);
			if (-1 == bytesSent)
			{
				throw SystemException();
			}
		}
	}

	void sendFile(const std::string & path)
	{
		std::ifstream ifs(path);
		std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		sendMessage(std::to_string(str.length()));
		ifs.close();
		sendMessage(str);
	}

	const std::string receiveMessage()
	{
		std::size_t netVarsSize = sizeof(uint16_t);
		std::vector<char> buffer(netVarsSize);
		ssize_t totalBytesReceived = 0;
		ssize_t bytesReceived = 0;
		while (totalBytesReceived < netVarsSize)
		{
			ssize_t diff = netVarsSize - totalBytesReceived;
			bytesReceived = ::recv(clientSocketFD_, &(buffer[0]), diff, 0);
			if (-1 == bytesReceived)
			{
				throw SystemException();
			}
			totalBytesReceived += bytesReceived;
		}
		uint16_t receivedNetMessageSize;
		memcpy(&receivedNetMessageSize, &(buffer[0]), netVarsSize);
		uint16_t receivedMessageSize = ntohs(receivedNetMessageSize);

		buffer.clear();
		buffer.resize(receivedMessageSize);
		totalBytesReceived = 0;
		while(totalBytesReceived < receivedMessageSize)
		{
			uint16_t diff = receivedMessageSize - (uint16_t)totalBytesReceived;
			uint16_t receive = (diff < MAX_MSG_SIZE) ? diff : MAX_MSG_SIZE;
			bytesReceived = ::recv(clientSocketFD_, &(buffer[0]), receive, 0);
			if (-1 == bytesReceived)
			{
				throw SystemException();
			}
			totalBytesReceived += bytesReceived;
		}
		return std::string(buffer.begin(), buffer.end());
	}

	void receiveFile(const std::string & path)
	{
		std::string fileSizeStr(receiveMessage());
		uint16_t fileSize = stoi(fileSizeStr);
		std::string fileStr(receiveMessage());
		std::ofstream ofs(path, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
		ofs.exceptions();
		ofs << fileStr;
		ofs.close();
	}

protected:
	int clientSocketFD_;
	static const uint16_t MAX_MSG_SIZE;
};

const uint16_t TCPSystem::MAX_MSG_SIZE = 1024u;

#endif