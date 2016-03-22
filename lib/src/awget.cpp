#include <cstdlib>			// EXIT_SUCCESS, EXIT_FAILURE
#include <iostream>			// cout endl
#include <fstream>			// ifstream
#include <memory>			// unique_ptr

#include "anonymization/ChainFile.hpp"
#include "anonymization/TCPClient.hpp"

void printUsage()
{
	std::cout << "Usage: awget url [-c chainfile]" << std::endl;
}

int main(int argc, char * argv[])
{
	std::unique_ptr<ChainFile> chainFile;
	switch(argc)
	{
		case 4:
		{
			std::string chainArg(argv[2]);
			if(chainArg == "-c")
			{
				std::string chainPath(argv[3]);
				std::ifstream chainStream(chainPath);
				chainFile = std::make_unique<ChainFile>(chainStream);
				chainStream.close();
			}
			else
			{
				printUsage();
				return EXIT_FAILURE;
			}
		}
		case 2:
		{
			std::string request(argv[1]);
			std::cout << "Request: " << request << std::endl;
			std::cout << "chainlist is" << std::endl;
			std::cout << chainFile->str() << std::endl;
			SSInfo stone = chainFile->removeRandomStone();
			std::cout << "next SS is " << stone.str() << std::endl;
			auto client = std::make_unique<TCPClient>(stone.getPort(), stone.getIp());
			client->sendMessage(request);
			std::cout << "Request sent." << std::endl;
			client->sendMessage(chainFile->str());
			std::cout << "Chainfile sent." << std::endl;
			std::cout << "waiting for file..." << std::endl;
			std::string s = "http://";
			std::string::size_type i = request.find(s);
			if (i != std::string::npos)
			{
			   request.erase(i, s.length());
			}
			int pos = request.find_last_of("/");
			client->receiveFile((pos != std::string::npos) ? request.substr(pos + 1) : "index.html");
			break;
		}
		default:
		{
			printUsage();
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}