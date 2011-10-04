#include "Module.h"
#include "ConfigReader.h"

#include <list>

using namespace std;

Module::Module(ConfigReader& x_configReader) :
	m_configReader(x_configReader)
{
};

void Module::Init()
{
	
	// Init images
	m_output = cvCreateImage(cvSize(GetRefParameter().width, GetRefParameter().height),
				 GetRefParameter().depth, GetRefParameter().channels);
}

Module::~Module()
{
	cvReleaseImage(&m_output);
};
