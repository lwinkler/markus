#include "Module.h"
#include "ConfigReader.h"

#include <list>

using namespace std;

Module::Module(ConfigReader& x_configReader, int width, int height, int depth, int channels) :
	m_configReader(x_configReader),
	m_width(width),
	m_height(height),
	m_depth(depth),
	m_channels(channels)
{
	m_output = cvCreateImage(cvSize(width, height), depth, channels);
	// Read config file
};

void Module::Init()
{
	GetRefParameter().SetDefault();
	ReadParametersFromConfig();
	GetRefParameter().CheckRange();
	cout<<"Module "<<GetName()<<" initialized."<<endl;
	GetRefParameter().PrintParameters();
}

Module::~Module()
{
	cvReleaseImage(&m_output);
};

void Module::ReadParametersFromConfig()
{
	m_configReader.ReadConfig(GetName());
	GetRefParameter().SetFromConfig(m_configReader.m_parameterList);
}