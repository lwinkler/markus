#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <tinyxml.h>
#include <string>
#include "Parameter.h"

class Module;
class Manager;

class ConfigReader
{
public:
	ConfigReader(const char * x_fileName)
	{
		m_fileName = x_fileName;
		m_parameterList.clear();
	};
	~ConfigReader(){};
	void ReadConfig(const std::string& x_moduleName);
	//void SetParameterValues(ParameterStructure& x_param);
	std::list<ParameterValue> m_parameterList;
private:
	std::string m_fileName;
};

#endif