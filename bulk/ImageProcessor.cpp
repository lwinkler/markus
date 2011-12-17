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
#include "SlitCam.h"
#include "ObjectTracker.h"
#include "CascadeDetector.h"

#include "util.h"
#include "UsbCam.h"
#include "VideoFileReader.h"

ImageProcessor::ImageProcessor(const string & x_name, int x_nb, ConfigReader& x_confReader, std::vector<Input*>& xr_inputList):
	m_param(x_confReader, x_nb), 
	Configurable(x_confReader),
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
	if(moduleClass.compare("SlitCamera") == 0)
	{
		m_module = new SlitCam(module.m_value, m_configReader);
	}
	else if(moduleClass.compare("ObjectTracker") == 0)
	{
		m_module = new ObjectTracker(module.m_value, m_configReader);
	}
	else if(moduleClass.compare("CascadeDetector") == 0)
	{
		m_module = new CascadeDetector(module.m_value, m_configReader);
	}
	else throw("Module type unknown : " + moduleClass);
	
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

	m_img_input = cvCreateImage( cvSize(m_module->GetWidth(), m_module->GetHeight()), m_module->GetDepth(), m_module->GetNbChannels());
	m_img_tmp1 = NULL; // Will be allocated on first call of adjust
	m_img_tmp2 = NULL;
	m_timeSinceLastProcessing = 0;
	m_timeInterval = 0;
	if(m_module->GetFps() > 0) m_timeInterval = 1.0 / m_module->GetFps();
}

ImageProcessor::~ImageProcessor()
{
	delete m_module;
	// delete m_input; // should not be deleted here
	if(m_img_tmp1 != NULL) cvReleaseImage(& m_img_tmp1);
	if(m_img_tmp2 != NULL) cvReleaseImage(& m_img_tmp2);
	if(m_img_input != NULL) cvReleaseImage(& m_img_input);
}

void ImageProcessor::Process(double x_timeSinceLast)
{
	m_timeSinceLastProcessing += x_timeSinceLast;
	//cout<<"Process "<<m_module->GetName()<<" "<<m_timeSinceLastProcessing<<" >= "<<m_timeInterval<<" "<<m_input->GetImage()<<endl;
	if(m_timeSinceLastProcessing >= m_timeInterval && m_input->GetImage() != NULL)
	{
		m_input->m_lock.lockForRead();
		adjust(m_input->GetImage(), m_img_input, m_img_tmp1, m_img_tmp2);
		m_input->m_lock.unlock();
		m_module->ProcessFrame(m_img_input, m_timeSinceLastProcessing);
		m_timeSinceLastProcessing = 0;
	}
}
