#include "ImageProcessor.h"
#include "SlitCam.h"
#include "ObjectTracker.h"

ImageProcessor::ImageProcessor(const string & x_name, int x_nb, ConfigReader& x_confReader):
	m_param(x_confReader, x_nb), 
	Configurable(x_confReader),
	m_nb(x_nb)
{
	vector<ParameterValue> paramList = m_configReader.ReadConfigObjectFromVect("ImageProcessors", "ImageProcessor", x_nb);
	ParameterValue module = ConfigReader::GetParameterValue("module", paramList);
	ParameterValue input  = ConfigReader::GetParameterValue("input" , paramList);

	// Create all modules types
	if(module.m_type.compare("SlitCamera") == 0)
	{
		m_module = new SlitCam(module.m_value, m_configReader);
	}
	else if(module.m_type.compare("ObjectTracker") == 0)
	{
		m_module = new ObjectTracker(module.m_value, m_configReader);
	}
	else throw("Module type unknown : " + module.m_type);
	
	// Create all input objects
	if (x_nb== 0)
		m_input = new Input(input.m_value, m_configReader);
}

void ImageProcessor::Process()
{

}
