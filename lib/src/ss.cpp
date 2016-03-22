#include <cstdlib>		// EXIT_SUCCESS, EXIT_FAILURE
#include <iostream>		// cout endl
#include <fstream>		// ifstream
#include <memory>		// unique_ptr
#include <cstdlib>		// system

#include "anonymization/ChainFile.hpp"
#include "anonymization/TCPServer.hpp"
#include "anonymization/TCPClient.hpp"

void printUsage()
{
	std::cout << "Usage: ss [-p port]" << std::endl;
}

int main(int argc, char * argv[])
{
	std::unique_ptr<TCPServer> server;
	switch(argc)
	{
		case 1:
		{
			server = std::make_unique<TCPServer>();
			break;
		}
		case 3:
		{
			std::string portFlag(argv[1]);
			if(portFlag == "-p")
			{
				uint16_t port = atoi(argv[2]);
				server = std::make_unique<TCPServer>(port);
				break;
			}
		}
		default:
		{
			printUsage();
			return EXIT_FAILURE;
		}
	}

	std::cout << "Running on:" << std::endl;
	std::cout << server->getHostname() << " port " << server->getPort() << std::endl;
	server->listen();
	std::string request = server->receiveMessage();
	std::cout << "Received request: " << request << std::endl;
	std::cout << "Waiting for chain file..." << std::endl;
	std::string chainStr = server->receiveMessage();
	std::istringstream chainStream(chainStr);
	auto chainFile = std::make_unique<ChainFile>(chainStream);
	std::cout << "chainlist is" << std::endl;
	std::cout << chainFile->str() << std::endl;
	if(!chainFile->empty())
	{
		SSInfo stone = chainFile->removeRandomStone();
		std::cout << "next SS is " << stone.str() << std::endl;
		auto client = std::make_unique<TCPClient>(stone.getPort(), stone.getIp());
		client->sendMessage(request);
		std::cout << "Request sent." << std::endl;
		client->sendMessage(chainFile->str());
		std::cout << "Chainfile sent." << std::endl;
		std::cout << "waiting for file..." << std::endl;
		server->sendMessage(client->receiveMessage());
	}
	else
	{
		std::string sysCommand("wget ");
		sysCommand.append(request);
		int get = std::system(&sysCommand[0]);
		std::string s = "http://";
		std::string::size_type i = request.find(s);
		if (i != std::string::npos)
		{
		   request.erase(i, s.length());
		}
		int pos = request.find_last_of("/");
		server->sendFile((pos != std::string::npos) ? request.substr(pos + 1) : "index.html");
	}
	return EXIT_SUCCESS;
}