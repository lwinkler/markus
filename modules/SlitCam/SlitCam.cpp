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
#include "StreamImage.h"

#include <iostream>
#include <cstdio>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;


SlitCam::SlitCam(const ConfigReader& x_configReader):
	Module(x_configReader), 
	m_param(x_configReader),
	m_input(Size(m_param.width, m_param.height), m_param.type),
	m_output(Size(m_param.width, m_param.height), m_param.type)
{
	m_description = "A simple example module that mimics a slit camera (or linear camera). The camera input is a range of pixels in the middle of the image.";
	m_position = 0;
	
	// Init output images
	m_inputStreams.push_back(new StreamImage(0, "input", m_input, *this, 	"Video input"));
	
	m_outputStreams.push_back(new StreamImage(0, "slit",  m_output, *this, 	"Slit camera stream"));
}

SlitCam::~SlitCam(void)
{
}

void SlitCam::Reset()
{
	Module::Reset();
	m_output.setTo(0);
}

void SlitCam::ProcessFrame()
{
	int aperture = m_param.aperture;
	m_position = m_position + aperture;
	m_position %= m_input.cols;
	int x = m_input.cols / 2;
	assert(aperture < x);
	for(int i = 0; i < aperture ; i++)
		if(m_position + i < m_input.cols)
			m_output.col(m_position + i) = m_input.col(x/* + i*/) * 1;
}

