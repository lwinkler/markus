#ifndef __MKEXCEPTIONS_H__
#define __MKEXCEPTIONS_H__

#include <exception>
#include <string>

#define S1(x) #x
#define S2(x) S1(x)
#define LOC __FILE__ " : " S2(__LINE__)

class MkException : public std::exception {
	public:
		MkException(const std::string& description="Exception occured", const std::string& position="");
		~MkException() throw();
		const char* what() const throw();

	protected:
		std::string descr;
};

class ParameterValueException : public MkException {
	public:
		ParameterValueException(const std::string& parameter, const std::string& position) : MkException("ParameterValueException: " + parameter, position){}
};

#endif
