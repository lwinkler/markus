#include "ImageProcessor.h"
#include "SlitCam.h"
#include "ObjectTracker.h"

ImageProcessor::ImageProcessor(const string & x_name, int x_nb, ConfigReader& x_confReader, std::vector<Input*>& xr_inputList):
	m_param(x_confReader, x_nb), 
	Configurable(x_confReader),
	m_nb(x_nb)
{
	cout<<"*** Create object ImageProcessor : "<<x_name<<" ("<<x_nb<<") ***"<<endl;
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
	//	check for similar existing input
	m_input = NULL;
	for(vector<Input*>::const_iterator it = xr_inputList.begin() ; it != xr_inputList.end() ; it++)
	{
		if((*it)->GetName().compare(m_param.input))
		{
			m_input = *it;
			break;
		}
	}
	
	if (m_input == NULL)
	{
		// Create new input
		m_input = new Input(input.m_value, m_configReader);
		xr_inputList.push_back(m_input);
	}
}

ImageProcessor::~ImageProcessor()
{
	delete m_module;
	// delete m_input; // should not be deleted
}


void ImageProcessor::Process()
{

}
