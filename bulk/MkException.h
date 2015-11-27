#ifndef __MKEXCEPTIONS_H__
#define __MKEXCEPTIONS_H__

#include <exception>
#include <string>
#include "define.h"
#include "Serializable.h"

void fatal(const std::string& x_description, const std::string& x_position, const std::string& x_function);

/**
* @brief Exception codes: correspond with return values (+1000)
*/
enum MkExceptionCode
{
	// initial value for exception codes
	MK_EXCEPTION_FIRST            = 1000,
	MK_EXCEPTION_NORMAL           = 1000, // normal exec must return 100 - 1000 = 0 for simulations
	MK_EXCEPTION_UNKNOWN          = 1011,
	MK_EXCEPTION_ENDOFSTREAM      = 1012,
	MK_EXCEPTION_PARAMETER        = 1013,
	MK_EXCEPTION_FEAT_NOT_FOUND   = 1014,
	// MK_EXCEPTION_FEAT_STD        = 1015,
	MK_EXCEPTION_WEBSERVICE       = 1015,
	MK_EXCEPTION_FATAL            = 1016,

	// last code since unix can only return codes from 0 to 126
	MK_EXCEPTION_LAST        = 1126
};

/**
* @brief Exception class
*/
class MkException : public std::exception, public Serializable
{
public:
	MkException(const std::string& x_description, const std::string& x_position, const std::string& x_function);
	MkException(MkExceptionCode x_code, const std::string& x_name, const std::string& x_description, const std::string& x_position, const std::string& x_function);
	~MkException() throw();
	const char* what() const throw();
	inline MkExceptionCode GetCode() const {return m_code;}
	inline const std::string& GetName() const {return m_name;}
	virtual void Serialize(std::ostream& stream, const std::string& x_dir) const;
	virtual void Deserialize(std::istream& stream, const std::string& x_dir);

protected:
	std::string m_description;
	std::string m_name;     // For interruption calls
	MkExceptionCode m_code; // For return value of main
};

/*class ProcessingException : public MkException {
	public:
		ProcessingException(const std::string& x_description, const std::string& x_position, const std::string& x_function="");
};

class FileNotFoundException : public MkException {
	public:
		FileNotFoundException(const std::string& x_description, const std::string& x_position, const std::string& x_function="");
};*/

/**
* @brief Exception thrown when a video file ends
*/
class EndOfStreamException : public MkException
{
public:
	EndOfStreamException(const std::string& x_description, const std::string& x_position, const std::string& x_function="");
};

/**
* @brief Exception thrown when a parameter has an incorrect value
*/
class ParameterException : public MkException
{
public:
	ParameterException(const std::string& x_description, const std::string& x_position, const std::string& x_function="");
};

/**
* @brief Exception thrown when a feature is not found in Object
*/
class FeatureNotFoundException : public MkException
{
public:
	FeatureNotFoundException(const std::string& x_description, const std::string& x_position, const std::string& x_function="");
};

/**
* @brief Exception thrown by a web service
*/
class WebServiceException : public MkException
{
public:
	WebServiceException(const std::string& x_description, const std::string& x_position, const std::string& x_function="");
};

/**
* @brief Fatal exception: will abort the execution
*/
class FatalException : public MkException
{
public:
	FatalException(const std::string& x_description, const std::string& x_position, const std::string& x_function="");
};


#endif
