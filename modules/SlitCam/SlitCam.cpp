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


const char * SlitCam::m_type = "SlitCamera";


SlitCam::SlitCam(const std::string& x_name, ConfigReader& x_configReader) 
	 : m_param(x_configReader, x_name), Module(x_name, x_configReader)
{
	m_position = 0;
	
	// Init images
	m_output = cvCreateImage(cvSize(m_param.width, m_param.height),
				 m_param.depth, m_param.channels);
	m_inputCopy = cvCreateImage(cvSize(m_param.width, m_param.height),
				 m_param.depth, m_param.channels);
	
	// Init output images
	m_outputStreams.push_back(new OutputStream("slit", STREAM_OUTPUT, m_output));
	m_outputStreams.push_back(new OutputStream("input", STREAM_OUTPUT, m_inputCopy));
}

SlitCam::~SlitCam(void)
{
	cvReleaseImage(&m_output);
	cvReleaseImage(&m_inputCopy);
	//TODO : delete output streams
}

void SlitCam::ProcessFrame(const IplImage * x_img)
{
	cvCopy(x_img, m_inputCopy);
	int widthStep = x_img->widthStep;
	int aperture = m_param.aperture;
	char * pDst = m_output->imageData + m_position * m_output->nChannels * aperture;// * x_img->nChannels;
	char * pSrc = x_img->imageData + x_img->widthStep * x_img->nChannels / 2;
	int size = m_output->depth * aperture;
	assert(m_output->imageSize == x_img->imageSize);
	
	for(int negCount = x_img->height ; negCount ; negCount--)
	{
		memcpy(pDst, pSrc, size);// * x_img->nChannels);
		//memset(pDst, *pSrc, m_image->depth);
		pSrc += widthStep;
		pDst += widthStep;
	}
	m_position = m_position == x_img->width - 1 ? 0 : m_position + 1;
}

