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

#include "SlitCam.h"

#include <iostream>
#include <cstdio>
#include <highgui.h>

using namespace std;
using namespace cv;


const char * SlitCam::m_type = "SlitCamera";


SlitCam::SlitCam(const std::string& x_name, ConfigReader& x_configReader) 
	 : m_param(x_configReader, x_name), Module(x_name, x_configReader)
{
	m_position = 0;
	
	// Init images
	m_input = new Mat(cvSize(m_param.width, m_param.height), m_param.type);
	m_output = new Mat(cvSize(m_param.width, m_param.height), m_param.type);
	m_output->setTo(cvScalar(0, 0, 0));
	
	// Init output images
	m_outputStreams.push_back(new StreamImage("slit",  m_output));
	m_outputStreams.push_back(new StreamImage("input", m_input));
}

SlitCam::~SlitCam(void)
{
	delete(m_output);
	//TODO : delete output streams
	for(vector<Stream *>::iterator it = m_outputStreams.begin() ; it != m_outputStreams.end() ; it++)
		delete *it;
	m_outputStreams.resize(0);
}

void SlitCam::ProcessFrame(const double /*x_timeSinceLastProcessing*/)
{
	int widthStep = m_input->cols;
	int aperture = m_param.aperture;
	/*unsigned char * pDst = m_output->datastart + m_position * m_output->channels() * aperture;// * x_img->nChannels;
	unsigned char * pSrc = x_img->datastart + x_img->cols * x_img->channels() / 2;
	int size = m_output->depth() * aperture;
	assert(m_output->size().area() == x_img->size().area());
	
	for(int negCount = x_img->rows ; negCount ; negCount--)
	{
		memcpy(pDst, pSrc, size);// * x_img->nChannels);
		//memset(pDst, *pSrc, m_image->depth);
		pSrc += widthStep;
		pDst += widthStep;
	}
	*/
	m_position = m_position + aperture;
	m_position %= m_input->cols;
	int x = m_input->cols / 2;
	assert(aperture < x);
	for(int i = 0; i < aperture ; i++)
		if(m_position + i < m_input->cols)
			m_output->col(m_position + i) = m_input->col(x/* + i*/) * 1;
}

