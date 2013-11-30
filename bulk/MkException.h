#ifndef __MKEXCEPTIONS_H__
#define __MKEXCEPTIONS_H__

#include <exception>
#include <string>

#define S1(x) #x
#define S2(x) S1(x)
// note: the next lines defines 2 parameters separated with a comma
#define LOC __FILE__ ":" S2(__LINE__), __FUNCTION__ 
#define POSITION __FILE__ ":" S2(__LINE__)

class Parameter;

class MkException : public std::exception {
	public:
		MkException(const std::string& x_description="Exception occured", const std::string& x_position="", const std::string& x_function="");
		~MkException() throw();
		const char* what() const throw();

	protected:
		std::string m_description;
};

class ParameterValueException : public MkException {
	public:
		ParameterValueException(const std::string& x_description, const std::string& x_position, const std::string& x_function="");
};

class ProcessingException : public MkException {
	public:
		ProcessingException(const std::string& x_description, const std::string& x_position, const std::string& x_function="");
};

class FileNotFoundException : public MkException {
	public:
		FileNotFoundException(const std::string& x_description, const std::string& x_position, const std::string& x_function="");
};

class EndOfStreamException : public MkException {
	public:
		EndOfStreamException(const std::string& x_description, const std::string& x_position, const std::string& x_function="");
};

/*class UnknownException : public MkException {
	public:
		UnknownException(const std::string& x_description, const std::string& x_position, const std::string& x_function="");
};*/
#endif
