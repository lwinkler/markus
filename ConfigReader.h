#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <tinyxml.h>
#include <cstring>

class Module;
class Manager;

class ConfigReader
{
public:
	ConfigReader(const char * x_fileName)
	{
		m_fileName = x_fileName;
	};
	~ConfigReader(){};
	void ReadConfig(Manager * x_module);
	void ReadConfig(Module * x_module);
	static const char * m_fileName;
private:
};

#endif