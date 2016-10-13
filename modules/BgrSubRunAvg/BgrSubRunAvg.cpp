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
#include <opencv2/opencv.hpp>

// for debug
#include "util.h"
using namespace cv;
using namespace std;

log4cxx::LoggerPtr BgrSubRunAvg::m_logger(log4cxx::Logger::getLogger("BgrSubRunAvg"));

BgrSubRunAvg::BgrSubRunAvg(ParameterStructure& xr_params) :
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_input(Size(m_param.width, m_param.height), m_param.type),
	m_background(Size(m_param.width, m_param.height), m_param.type),
	m_foreground(Size(m_param.width, m_param.height), m_param.type),
	m_foreground_tmp(Size(m_param.width, m_param.height), m_param.type),
	m_accumulator()
{
	AddInputStream(0, new StreamImage("image",             m_input, *this,   "Video input"));

	AddOutputStream(0, new StreamImage("foreground", m_foreground,*this,      "Foreground"));
	AddOutputStream(1, new StreamImage("background", m_background, *this,		"Background"));

	AddDebugStream(0, new StreamImage("foreground_tmp", m_foreground_tmp,*this,      "Foreground tmp"));
};


BgrSubRunAvg::~BgrSubRunAvg()
{
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
	if(m_emptyBackgroundSubtractor)
	{
		m_emptyBackgroundSubtractor = false;
		m_input.convertTo(m_accumulator, CV_32F);
		m_input.copyTo(m_background);
	}

	// Main part of the program
	accumulateWeighted(m_input, m_accumulator, m_param.backgroundAlpha);
	m_accumulator.convertTo(m_background, m_background.depth());
	absdiff(m_input, m_background, m_foreground_tmp);
	threshold(m_foreground_tmp, m_foreground, m_param.foregroundThres * 255, 255, cv::THRESH_BINARY);
};

