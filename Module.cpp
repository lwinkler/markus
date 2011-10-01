#include "Module.h"
#include "ConfigReader.h"

Module::Module(ConfigReader& x_configReader, int width, int height, int depth, int channels) :
m_configReader(x_configReader)
{
	m_output = cvCreateImage(cvSize(width, height), depth, channels);
	// Read config file
};

void Module::Init()
{
	m_configReader.ReadConfig(this);
}



Module::~Module()
{
	cvReleaseImage(&m_output);
};