#ifndef __MKEXCEPTIONS_H__
#define __MKEXCEPTIONS_H__

#include <exception>
#include <string>

#define S1(x) #x
#define S2(x) S1(x)
#define LOC __FILE__ " : " S2(__LINE__)

class Parameter;

class MkException : public std::exception {
	public:
		MkException(const std::string& x_description="Exception occured", const std::string& x_position="");
		~MkException() throw();
		const char* what() const throw();

	protected:
		std::string m_description;
};

class ParameterValueException : public MkException {
	public:
		ParameterValueException(const std::string& x_description, const std::string& x_position);
};

class ProcessingException : public MkException {
	public:
		ProcessingException(const std::string& x_description, const std::string& x_position);
};

class FileException : public MkException {
	public:
		FileException(const std::string& x_description, const std::string& x_position);
};

class EndOfStreamException : public MkException {
	public:
		EndOfStreamException(const std::string& x_description, const std::string& x_position);
};

class UnknownException : public MkException {
	public:
		UnknownException(const std::string& x_description, const std::string& x_position);
};
#endif
