#include "MkException.h"
#include "Parameter.h"
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

using namespace std;

// Write the description of an exception
string writeDescription(const std::string& x_description, const std::string& x_position, const std::string& x_function)
{
	string description;
	if (x_position == "")
	{
		description = x_description + " [position unknown]";
	}
	else
	{
		const char * basename = strrchr(x_position.c_str(), '/');
		if(basename == NULL)
			description = x_description + " [" + x_position;
		else 
			description = x_description + " [" + (basename + 1);
	}
	if (x_function == "")
	{
		description += "]";
	}
	else
	{
		description += " in " + x_function + "]";
	}
	return description;
}

MkException::MkException(const std::string& x_description, const std::string& x_position, const std::string& x_function)
{
	m_description = writeDescription(x_description, x_position, x_function);
	m_code = MK_EXCEPTION_UNKNOWN;
}

MkException::MkException(MkExceptionCode x_code, const std::string& x_description, const std::string& x_position, const std::string& x_function)
{
	m_description = writeDescription(x_description, x_position, x_function);
	m_code = x_code;
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
	MkException(MK_EXCEPTION_ENDOFSTREAM, "EndOfStreamException: " + x_descr, x_position)
{}

ParameterException::ParameterException(const std::string& x_descr, const std::string& x_position, const std::string& x_function) : 
	MkException(MK_EXCEPTION_PARAMETER, "ParameterException: " + x_descr, x_position)
{}

FeatureNotFoundException::FeatureNotFoundException(const std::string& x_descr, const std::string& x_position, const std::string& x_function) : 
	MkException(MK_FEAT_NOT_FOUND_PARAMETER, "FeatureNotFoundException: " + x_descr, x_position)
{}

/// Serialize exception to JSON
/// @param x_out Output stream
/// @param x_dir Output directory (for images)
void MkException::Serialize(std::ostream& x_out, const string& x_dir) const
{
	Json::Value root;
	root["description"] = m_description;
	root["code"] = m_code;
	x_out << root;
}

/// Deserialize the event from JSON
/// @param x_in Input stream
/// @param x_dir Input directory (for images)
void MkException::Deserialize(std::istream& x_in, const string& x_dir)
{
	// TODO
	// Note that a null JSON means that the event was not raised
}
