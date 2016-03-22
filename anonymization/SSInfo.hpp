#ifndef STEPPING_STONE_H
#define STEPPING_STONE_H

#include <string>

class SSInfo
{
public:
	static const std::string DELIMITER;
	
	SSInfo(const std::string & ip, const std::string & port)
	: ip_(ip), port_(port)
	{}

	std::string getIp()
	{
		return ip_;
	}

	std::string getPort()
	{
		return port_;
	}

	std::string str()
	{
		return std::string(getIp() + DELIMITER + getPort());
	}

private:
	std::string ip_;
	std::string port_;
};

const std::string SSInfo::DELIMITER = ", ";

#endif