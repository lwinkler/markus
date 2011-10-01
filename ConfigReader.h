#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <tinyxml.h>
#include <cstring>

class Module;
class Manager;

class ConfigReader
{
public:
	ConfigReader(std::string x_fileName)
	{
		m_fileName = x_fileName;
	};
	~ConfigReader(){};
	void ReadConfig(Manager * x_module);
	void ReadConfig(Module * x_module);
private:
	std::string m_fileName;
};

#endif