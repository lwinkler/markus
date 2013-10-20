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

#include "BgrSubRunAvg.h"
#include "StreamImage.h"

// for debug
#include "util.h"
using namespace cv;
using namespace std;

const char * BgrSubRunAvg::m_type = "BgrSubRunAvg";


BgrSubRunAvg::BgrSubRunAvg(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader)
{
	m_tmp1 = new Mat();
	m_tmp2 = NULL;
	m_description = "Perform a background subtraction using a running average";
	m_input    = new Mat(Size(m_param.width, m_param.height), m_param.type);
	m_background 		= new Mat(Size(m_param.width, m_param.height), m_param.type);
	m_foreground 		= new Mat(Size(m_param.width, m_param.height), m_param.type);
	m_foreground_tmp	= new Mat(Size(m_param.width, m_param.height), m_param.type);
	
	m_inputStreams.push_back(new StreamImage(0, "input",             m_input, *this,   "Video input"));
	m_outputStreams.push_back(new StreamImage(0, "foreground", m_foreground,*this,      "Foreground"));
	m_outputStreams.push_back(new StreamImage(1, "background", m_background, *this,		"Background"));
	m_debugStreams.push_back(new StreamImage(0, "foreground_tmp", m_foreground_tmp,*this,      "Foreground tmp"));
};
		

BgrSubRunAvg::~BgrSubRunAvg()
{
	delete(m_input);
	delete(m_background);
	delete(m_foreground);
	delete(m_tmp1);
	delete(m_tmp2);
}

void BgrSubRunAvg::Reset()
{
	Module::Reset();
	m_emptyBackgroundSubtractor = true;
}

void BgrSubRunAvg::ProcessFrame()
{
	// cout<<"input "<<m_input->size()<<" "<<m_input->depth();
	// cout<<"backgr "<<m_background->size()<<" "<<m_background->depth();
	if(m_emptyBackgroundSubtractor) {
		//m_input->copyTo(*m_background);
		// accumulateWeighted(*m_input, *m_background, 1);
		m_emptyBackgroundSubtractor = false;
		m_input->convertTo(*m_tmp1, CV_32F);
		m_input->copyTo(*m_background); // TODO: See why images in 32F such as tmp1 cannot be displayed correctly
	}
	// Main part of the program
	accumulateWeighted(*m_input, *m_tmp1, m_param.backgroundAlpha);
	m_tmp1->convertTo(*m_background, m_background->depth());
	absdiff(*m_input, *m_background, *m_foreground_tmp);
	threshold(*m_foreground_tmp, *m_foreground, m_param.foregroundThres * 255, 255, cv::THRESH_BINARY);
};

