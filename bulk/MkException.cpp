#include "MkException.h"
#include "Parameter.h"

using namespace std;

MkException::MkException(const std::string& x_description, const std::string& x_position, const std::string& x_function)
{
	if (x_position == "")
	{
		m_description = x_description + " [position unknown]";
	}
	else
	{
		const char * basename = strrchr(x_position.c_str(), '/');
		if(basename == NULL)
			m_description = x_description + " [" + x_position;
		else 
			m_description = x_description + " [" + (basename + 1);
	}
	if (x_function == "")
	{
		m_description += "]";
	}
	else
	{
		m_description += " in " + x_function + "]";
	}
}

MkException::~MkException() throw(){
}


const char* MkException::what() const throw()
{
	return m_description.c_str();
}



// ProcessingException::ProcessingException(const std::string& x_descr, const std::string& x_position, const std::string& x_function) : 
	// MkException("ProcessingValueException: " + x_descr, x_position)
// {}

// FileNotFoundException::FileNotFoundException(const std::string& x_descr, const std::string& x_position, const std::string& x_function) : 
	// MkException("FileException: " + x_descr, x_position)
// {}

EndOfStreamException::EndOfStreamException(const std::string& x_descr, const std::string& x_position, const std::string& x_function) : 
	MkException("EndOfStreamException: " + x_descr, x_position)
{}

ParameterException::ParameterException(const std::string& x_descr, const std::string& x_position, const std::string& x_function) : 
	MkException("ParameterException: " + x_descr, x_position)
{}
