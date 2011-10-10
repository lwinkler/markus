#include "Module.h"
#include "ConfigReader.h"

#include <list>

using namespace std;

Module::Module(const std::string& x_name, ConfigReader& x_configReader) :
	Configurable(x_configReader),
	m_name(x_name)
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
