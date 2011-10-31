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
#include "util.h"

ImageProcessor::ImageProcessor(const string & x_name, int x_nb, ConfigReader& x_confReader, std::vector<Input*>& xr_inputList):
	m_param(x_confReader, x_nb), 
	Configurable(x_confReader),
	m_nb(x_nb)
{
	cout<<"*** Create object ImageProcessor : "<<x_name<<" ["<<x_nb<<"] ***"<<endl;
	vector<ParameterValue> paramList = m_configReader.ReadConfigObjectFromVect("ImageProcessors", "ImageProcessor", x_nb);
	ParameterValue module = ConfigReader::GetParameterValue("module", paramList);
	ParameterValue input  = ConfigReader::GetParameterValue("input" , paramList);

	// Create all modules types
	if(module.m_class.compare("SlitCamera") == 0)
	{
		m_module = new SlitCam(module.m_value, m_configReader);
	}
	else if(module.m_class.compare("ObjectTracker") == 0)
	{
		m_module = new ObjectTracker(module.m_value, m_configReader);
	}
	else throw("Module type unknown : " + module.m_class);
	
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

	m_img_input = cvCreateImage( cvSize(m_module->GetWidth(), m_module->GetHeight()), m_module->GetDepth(), m_module->GetNbChannels());
	m_img_tmp1 = NULL; // Will be allocated on forst call of adjust
	m_img_tmp2 = NULL;
}

ImageProcessor::~ImageProcessor()
{
	delete m_module;
	// delete m_input; // should not be deleted here
	cvReleaseImage(& m_img_tmp1);
	cvReleaseImage(& m_img_tmp2);
	cvReleaseImage(& m_img_input);
}

void ImageProcessor::Process()
{
	adjust(m_input->GetImage(), m_img_input, m_img_tmp1, m_img_tmp2);
	m_module->ProcessFrame(m_img_input);
}
