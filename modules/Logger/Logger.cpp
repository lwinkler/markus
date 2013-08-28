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

#include "Logger.h"
#include "StreamDebug.h"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <highgui.h>


using namespace std;
using namespace cv;


const char * Logger::m_type = "Logger";


Logger::Logger(const ConfigReader& x_configReader) 
	 : Module(x_configReader), m_param(x_configReader)
{
	m_description = "This module takes a state as input and logs it to .srt file";
	
	// Init images
	// m_input = new Mat(cvSize(m_param.width, m_param.height), m_param.type);
	
	// Init output images
	// m_inputStreams.push_back(new StreamImage(0, "input", m_input, *this, 	"Video input"));
	//m_outputStreams.push_back(new StreamImage(0, "slit",  m_output, *this, 	"Slit camera stream"));
}

Logger::~Logger(void)
{
	// delete(m_input);
}

void Logger::Reset()
{
	Module::Reset();
	m_timer.Restart();
	m_startTime = "00:00:00,000";
	time_t now = time(0);
	m_srtFileName = "log." + string(ctime(&now)) + ".srt";
	m_status = 0;
	m_subId = 0;
}

void Logger::ProcessFrame()
{
	bool newStatus = 0; // (val >= m_param.motionThres);

	if(m_status != newStatus) {
		// Log the change in status
		ofstream myfile;

		long t  = m_timer.GetMSecLong();
		int msecs = t % 1000;
		t /= 1000;
		int secs = t % 60; 
		t /= 60;
		int mins = t % 60; 
		t /= 60;
		int hours = t; 

		char str[32];
		// str<<hours<<":"<<min<<":"<<secs<<","<<msecs;
		sprintf(str, "%02d:%02d:%02d,%03d", hours, mins, secs, msecs);

		myfile.open (m_srtFileName.c_str(), std::ios_base::app);
		myfile<<m_subId<<endl;
		myfile<<m_startTime<<" --> "<<str<<endl;
		myfile<<"state_"<<newStatus<<endl<<endl;

		myfile.close();
		m_startTime = str;
		m_status = newStatus;
		m_subId++;
	}
}

