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
#include "StreamState.h"
#include "util.h"

#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>

using namespace std;
using namespace cv;


const char * Logger::m_type = "Logger";


Logger::Logger(const ConfigReader& x_configReader) 
	 : Module(x_configReader), m_param(x_configReader)
{
	m_description = "This module takes a state as input and logs it to .srt file";
	
	// Init input images
	m_inputStreams.push_back(new StreamState(0, "input", m_state, *this, 	"Input state to be logged"));
}

Logger::~Logger(void)
{
	// delete(m_input);
}

void Logger::Reset()
{
	Module::Reset();
	m_state = m_oldState = 0;
	m_subId = 0;
	m_startTime = "00:00:00,000";
	
	// write time stamp in log filename
	/*time_t now = time(0);
	struct tm tm_struct;
	assert(gmtime(&tm_struct, &now) == 0);
	m_srtFileName = "log." + gmtime(&tm_struct, &now) + ".srt";
	*/


	m_srtFileName = m_param.file + (m_param.timeStamp ? "." + timeStamp() : "") + ".srt";
	// cout<<"file"<<m_param.file<<endl;
	// cout<<"timeStamp"<<timeStamp()<<endl;
	// cout<<m_srtFileName<<endl;
}

void Logger::ProcessFrame()
{
	if(m_state != m_oldState) {
		LOG_DEBUG("state change");
		// Log the change in state
		ofstream myfile;

		long t  = static_cast<long>(m_currentTimeStamp); // m_timer.GetMSecLong();
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
		myfile<<"state_"<<m_oldState<<endl<<endl;

		myfile.close();
		m_startTime = str;
		m_oldState = m_state;
		m_subId++;
	}
}

