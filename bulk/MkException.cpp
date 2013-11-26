#include "MkException.h"
using namespace std;

MkException::MkException(const std::string& description, const std::string& position)
{
	if (position=="") {
		descr = "At unknown position: " + description;
	} else {
		descr = position + ": " + description;
	}
}

MkException::~MkException() throw(){
}


const char* MkException::what() const throw()
{
	return descr.c_str();
}
