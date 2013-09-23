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

#include "LogObjects.h"
#include "StreamState.h"
#include "util.h"

#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>

using namespace std;
using namespace cv;


const char * LogObjects::m_type = "LogObjects";


LogObjects::LogObjects(const ConfigReader& x_configReader) 
	 : Module(x_configReader), m_param(x_configReader)
{
	m_description = "This module takes a stream of objects as inputs and logs data to a text file";
	
	// Init input streams
	m_inputStreams.push_back(new StreamObject(0, "input",      m_param.width, m_param.height, m_objectsIn, cvScalar(255, 255, 255), *this,     "Incoming objects"));
}

LogObjects::~LogObjects(void)
{
	// delete(m_input);
}

void LogObjects::Reset()
{
	Module::Reset();

	m_fileName = m_param.file + (m_param.timeStamp ? "." + timeStamp() : "") + ".txt";
}

void LogObjects::ProcessFrame()
{
	ofstream myfile;
	myfile.open (m_fileName.c_str(), std::ios_base::app);
	
	// Log time stamp
	myfile<<"## "<<m_currentTimeStamp<<endl;

	// Log each feature of every object into a text file
	for(vector<Object>::const_iterator it1 = m_objectsIn.begin() ; it1 != m_objectsIn.end() ; it1++)
	{
		myfile<<it1->GetName()<<it1->GetId()<<endl;
		for(map<string, Feature>::const_iterator it2 = it1->GetFeatures().begin() ; it2 != it1->GetFeatures().end() ; it2++)
		{
			const Feature & feat = it2->second;
			myfile<<it2->first<<":\t"
				<<feat.value<<"\t"
				<<feat.mean<<"\t"
				<<feat.sqVariance<<"\t"
				<<feat.initial<<"\t"
				<<feat.min<<"\t"
				<<feat.max<<"\t"
				<<feat.nbSamples<<endl;
		}
		myfile<<endl;
	}
	myfile.close();
}

