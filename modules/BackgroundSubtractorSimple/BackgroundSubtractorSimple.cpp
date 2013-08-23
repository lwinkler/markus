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

#include "BackgroundSubtractorSimple.h"
#include "StreamDebug.h"
#include "StreamImage.h"

// for debug
#include "util.h"
/*#include "cv.h"
#include "highgui.h"
*/
using namespace cv;

const char * BackgroundSubtractorSimple::m_type = "BackgroundSubtractorSimple";


BackgroundSubtractorSimple::BackgroundSubtractorSimple(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader)
{
	m_tmp1 = NULL;
	m_tmp2 = NULL;
	m_description = "Track moving objects on video by using backgroung subtraction.";
	m_input    = new Mat(cvSize(m_param.width, m_param.height), m_param.type);
	m_background 		= new Mat(cvSize(m_param.width, m_param.height), CV_32F);
	m_foreground 		= new Mat(cvSize(m_param.width, m_param.height), CV_8UC1);
	m_foreground_tmp	= new Mat(cvSize(m_param.width, m_param.height), CV_8UC1);
	
	m_inputStreams.push_back(new StreamImage(0, "input",             m_input, *this,   "Video input"));
	m_outputStreams.push_back(new StreamImage(0, "foreground", m_foreground,*this,      "Foreground"));
	m_outputStreams.push_back(new StreamImage(1, "background", m_background, *this,		"Background"));
	m_debugStreams.push_back(new StreamImage(0, "foreground_tmp", m_foreground_tmp,*this,      "Foreground tmp"));
	m_debugStreams.push_back(new StreamImage(1, "input", m_input,*this,      "input tmp"));
	m_debugStreams.push_back(new StreamImage(2, "background tmp", m_background,*this,      "background tmp"));
};
		

BackgroundSubtractorSimple::~BackgroundSubtractorSimple()
{
	delete(m_input);
	delete(m_background);
	delete(m_foreground);
	delete(m_tmp1);
	delete(m_tmp2);
}

void BackgroundSubtractorSimple::Reset()
{
	Module::Reset();
	m_emptyBackgroundSubtractor = true;
}

void BackgroundSubtractorSimple::ProcessFrame()
{
	if(m_emptyBackgroundSubtractor) {
		//m_input->copyTo(*m_background);
		adjust(m_input, m_background, m_tmp1, m_tmp2);
		m_emptyBackgroundSubtractor = false;
	}
	// Main part of the program
	cv:accumulateWeighted(*m_input, *m_background, 0); // m_param.backgroundAlpha);
	//cv::subtract(*m_input, *m_background, *m_foreground_tmp);
	//cv::threshold(*m_foreground_tmp, *m_foreground_tmp, m_param.foregroundThres, 255, cv::THRESH_BINARY);
};

