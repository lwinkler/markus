#include "MkException.h"
#include "Parameter.h"

using namespace std;

MkException::MkException(const std::string& x_description, const std::string& x_position)
{
	if (x_position=="")
	{
		m_description = x_description + " [position unknown]";
	}
	else
	{
		const char * basename = strrchr(x_position.c_str(), '/');
		if(basename == NULL)
			m_description = x_description + " [" + x_position + "]";
		else 
			m_description = x_description + " [" + (basename + 1) + "]";
	}
}

MkException::~MkException() throw(){
}


const char* MkException::what() const throw()
{
	return m_description.c_str();
}



ParameterValueException::ParameterValueException(const std::string& x_descr, const std::string& x_position) : 
	MkException("ParameterValueException: " + x_descr, x_position)
{}

ProcessingException::ProcessingException(const std::string& x_descr, const std::string& x_position) : 
	MkException("ProcessingValueException: " + x_descr, x_position)
{}

FileException::FileException(const std::string& x_descr, const std::string& x_position) : 
	MkException("FileException: " + x_descr, x_position)
{}

EndOfStreamException::EndOfStreamException(const std::string& x_descr, const std::string& x_position) : 
	MkException("EndOfStreamException: " + x_descr, x_position)
{}

UnknownException::UnknownException(const std::string& x_descr, const std::string& x_position) : 
	MkException("UnknownException: " + x_descr, x_position)
{}
