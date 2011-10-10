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
	void ReadConfigModule(const std::string& x_moduleName);
	void ReadConfigModules();
	std::string GetParameterValue(const std::string& x_name) const;
	//void SetParameterValues(ParameterStructure& x_param);
	std::list<ParameterValue> m_parameterList;
private:
	std::string m_fileName;
};

class Configurable
{
public:
	Configurable(ConfigReader& x_confReader) : m_configReader(x_confReader){};
	~Configurable(){};
protected:
	void ReadParametersFromConfig();
	ConfigReader& m_configReader;
	virtual const ParameterStructure & GetRefParameter() = 0;
};

#endif