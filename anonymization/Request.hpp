

class Request
{
public:
	Request(const std::string & str)
	: request_(str),
	  filename_(parseFileName(request_))
	{

	}



private:
	std::string request_;
	std::string filename_;

	const std::string parseFileName(const std::string & str)
	{
		if()
		{
			
		}
	}
};