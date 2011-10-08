#include "Module.h"
#include "ConfigReader.h"

#include <list>

using namespace std;

Module::Module(ConfigReader& x_configReader) :
	m_configReader(x_configReader)
{
};


Module::~Module()
{
};

void Module::AddStream(const std::string& x_name, StreamType x_type, IplImage* x_image)
{
	OutputStream stream(x_name, x_type, x_image);
	m_outputStreams.push_back(stream);
}
