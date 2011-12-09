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

#include "FaceDetector.h"

#include <iostream>
#include <cstdio>
#include <highgui.h>

using namespace std;


const char * FaceDetector::m_type = "FaceDetector";


FaceDetector::FaceDetector(const std::string& x_name, ConfigReader& x_configReader) 
	 : m_param(x_configReader, x_name), ModuleAsync(x_name, x_configReader)
{
	
	// Init output images
//	m_outputStreams.push_back(new OutputStream("slit", STREAM_OUTPUT, m_output));
//	m_outputStreams.push_back(new OutputStream("input", STREAM_OUTPUT, m_inputCopy));
}

FaceDetector::~FaceDetector(void)
{
	//TODO : delete output streams
}

void FaceDetector::ThreadProcess(const IplImage* img, const double x_timeSinceLastProcessing)
{

}

