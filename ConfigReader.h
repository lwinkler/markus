#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <tinyxml.h>
#include <string>
#include "Parameter.h"
#include <vector>


class Module;
//class Manager;std::vector<ParameterValue> 

class ConfigReader
{
public:
	ConfigReader(const char * x_fileName)
	{
		m_fileName = x_fileName;
		//m_parameterList.clear();
	};
	~ConfigReader(){};
	std::vector<ParameterValue> ReadConfigModule(const std::string& x_moduleName);
	std::vector<ParameterValue> ReadConfigDetectors(int x_detectorNumber);
	static ParameterValue GetParameterValue(const std::string& x_name, const std::vector<ParameterValue> & x_parameterList);
	//void SetParameterValues(ParameterStructure& x_param);
private:
	//std::vector<ParameterValue> m_parameterList;
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