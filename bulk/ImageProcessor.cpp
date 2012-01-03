/*----------------------------------------------------------------------------------
*
*    MARKUS : a manager for video analysis modules
* 
*    author : Laurent Winkler <lwinkler888@gmail.com>
* 
* 
*    This file is part of Markus.
*
*    Markus is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Markus is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with Markus.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------------*/

#include "ImageProcessor.h"
#include "AllModules.h"
#include "AllInputs.h"

#include "util.h"

using namespace std;
using namespace cv;

ImageProcessor::ImageProcessor(const string & x_name, int x_nb, ConfigReader& x_confReader, std::vector<Input*>& xr_inputList):
	Configurable(x_confReader),
	m_param(x_confReader, x_nb), 
	m_nb(x_nb)
{
	vector<ParameterValue> paramList = m_configReader.ReadConfigObjectFromVect("ImageProcessors", "ImageProcessor", x_nb);
	ParameterValue module = ConfigReader::GetParameterValue("module", paramList);
	ParameterValue input  = ConfigReader::GetParameterValue("input" , paramList);
	
	// Get the object class
	paramList = m_configReader.ReadConfigObject("Module", module.m_value, true);
	assert(paramList.size() == 1);
	std::string moduleClass = paramList[0].m_value;
	
	// Create all modules types
	Module * tmp;
	if(moduleClass.compare("SlitCamera") == 0)
	{
		tmp = new SlitCam(module.m_value, m_configReader);
	}
	else if(moduleClass.compare("ObjectTracker") == 0)
	{
		tmp = new ObjectTracker(module.m_value, m_configReader);
	}
	else if(moduleClass.compare("CascadeDetector") == 0)
	{
		tmp = new CascadeDetector(module.m_value, m_configReader);
	}
	else throw("Module type unknown : " + moduleClass);

	m_modules.push_back(tmp);
		//}
	
	// Create all input objects
	//	check for similar existing input
	paramList = m_configReader.ReadConfigObject("Module", input.m_value, true);
	assert(paramList.size() == 1);
	std::string inputClass = paramList[0].m_value;
	
	m_input = NULL;
	for(vector<Input*>::const_iterator it = xr_inputList.begin() ; it != xr_inputList.end() ; it++)
	{
		if((*it)->GetName().compare(input.m_value) == 0)
		{
			m_input = *it;
			break;
		}
	}
	
	if (m_input == NULL)
	{
		// Create new input
		if(inputClass.compare("UsbCam") == 0)
		{
			m_input = new UsbCam(input.m_value, m_configReader);
		}
		else if(inputClass.compare("VideoFileReader") == 0)
		{
			m_input = new VideoFileReader(input.m_value, m_configReader);
		}
		else throw("Input type unknown : " + inputClass);

		xr_inputList.push_back(m_input);
	}
	
	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
	{
		for(vector<Stream*>::iterator it2 = (*it)->m_inputStreams.begin() ; it2 != (*it)->m_inputStreams.end() ; it2++)
		{
			(*it2)->Connect(m_input->m_outputStreams[0]);
		}
	}
	
	m_timeSinceLastProcessing = 0;
	//m_timeInterval = 0;
	//if(m_module->GetFps() > 0) m_timeInterval = 1.0 / m_module->GetFps();
}

ImageProcessor::~ImageProcessor()
{
	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
		delete *it;
	// delete m_input; // should not be deleted here
	//if(m_img_input != NULL) delete(m_img_input);
}

void ImageProcessor::Process(double x_timeSinceLast)
{
	m_lock.lockForWrite();
	m_timeSinceLastProcessing += x_timeSinceLast;
	//cout<<"Process "<<m_module->GetName()<<" "<<m_timeSinceLastProcessing<<" >= "<<m_timeInterval<<" "<<m_input->GetImage()<<endl;
	for(vector<Module*>::iterator it = m_modules.begin() ; it != m_modules.end() ; it++)
	{
		assert(m_input->GetImage() != NULL);
		if(m_timeSinceLastProcessing >= 1.0 / (*it)->GetFps())
		{
			m_input->m_lock.lockForRead();
			(*it)->ConvertInput(m_input->GetImage());
			m_input->m_lock.unlock();
			(*it)->ProcessFrame(m_timeSinceLastProcessing);
			m_input->ProcessFrame(m_timeSinceLastProcessing);
			m_timeSinceLastProcessing = 0;
		}
	}
	m_lock.unlock();
}
