#ifndef CHAIN_FILE_H
#define CHAIN_FILE_H

#include <string>
#include <vector>
#include <istream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stdexcept>
#include <sstream>			// stringstream

#include "SSInfo.hpp"

class ChainFile
{
public:
	ChainFile(std::istream & stream)
	: numStones_(parseNumStones(stream)),
	  steppingStones_(parseSteppingStones(stream))
	{
		if(numStones_ != steppingStones_.size())
		{
			throw std::logic_error("Improperly formatted ChainFile. Number of stones does not match count.");
		}
	}

	SSInfo removeRandomStone()
	{
		if(steppingStones_.empty())
		{
			throw std::logic_error("Cannot get a random stone from an empty ChainFile.");
		}
		std::srand(std::time(nullptr));
		int randN = std::rand() % numStones_;
		SSInfo stone = steppingStones_[randN];
		steppingStones_.erase(std::next(steppingStones_.begin(), randN));
		numStones_--;
		return stone;
	}

	const uint16_t numStones()
	{
		return numStones_;
	}

	const std::vector<SSInfo> steppingStones()
	{
		return steppingStones_;
	}

	bool empty()
	{
		return numStones() == 0;
	}

	const std::string str()
	{
		std::ostringstream chain;
		chain << numStones_ << "\n";
		for(auto && stone : steppingStones_)
		{
			chain << stone.str() << "\n";
		}
		return chain.str();
	}

private:
	uint16_t numStones_;
	std::vector<SSInfo> steppingStones_;

	int parseNumStones(std::istream & stream)
	{
		std::string numBuff;
		std::getline(stream, numBuff);
		return std::stoi(numBuff);
	}

	std::vector<SSInfo> parseSteppingStones(std::istream & stream)
	{
		std::vector<SSInfo> stones;
		std::string lineBuff;
		std::string ip;
		std::string port;
		while(std::getline(stream, lineBuff))
		{
			int index = lineBuff.find(SSInfo::DELIMITER);
			if(index == -1)
			{
				throw std::invalid_argument("The given ChainFile is improperly formatted.");
			}
			ip = lineBuff.substr(0, index);
			port = lineBuff.substr(index + SSInfo::DELIMITER.length(), lineBuff.length());
			stones.push_back(SSInfo(ip, port));
		}
		return stones;
	}
};

#endif