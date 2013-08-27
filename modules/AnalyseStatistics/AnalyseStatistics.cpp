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

#include "AnalyseStatistics.h"
#include "StreamImage.h"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <highgui.h>


using namespace std;
using namespace cv;


const char * AnalyseStatistics::m_type = "AnalyseStatistics";


AnalyseStatistics::AnalyseStatistics(const ConfigReader& x_configReader) 
	 : Module(x_configReader), m_param(x_configReader)
{
	m_description = "This modules analyses some statistics as input and save states as result";
	
	// Init images
	m_input = new Mat(cvSize(m_param.width, m_param.height), m_param.type);
	
	// Init output images
	m_inputStreams.push_back(new StreamImage(0, "input", m_input, *this, 	"Video input"));
	//m_outputStreams.push_back(new StreamImage(0, "slit",  m_output, *this, 	"Slit camera stream"));
}

AnalyseStatistics::~AnalyseStatistics(void)
{
	delete(m_input);
}

void AnalyseStatistics::Reset()
{
	Module::Reset();
	m_status = 0;
	m_subId = 0;
}

void AnalyseStatistics::ProcessFrame()
{
	assert(m_input->channels() == 1); // Currently only supporting 1 channel

	vector<Mat> channels;
	split(*m_input, channels);
	Scalar m = mean(channels[0]);

	m /= 255.0;
	float val = sum(m)[0];
	bool newStatus = (val >= m_param.motionThres);

	if(m_status != newStatus) {
		// Log the change in status
		ofstream myfile;

		myfile.open ("log.srt", std::ios_base::app);
		myfile<<m_subId<<endl<<endl;
		myfile<<"00:03:24,350 –> 00:03:28,800"<<endl<<endl;
		myfile<<"state_"<<newStatus<<endl<<endl;

		myfile.close();
		m_status = newStatus;
	}

}

