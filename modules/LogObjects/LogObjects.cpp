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
#include "Manager.h"

#include <ctime>

#define SEP "\t"


using namespace std;
using namespace cv;

log4cxx::LoggerPtr LogObjects::m_logger(log4cxx::Logger::getLogger("LogObjects"));

LogObjects::LogObjects(const ConfigReader& x_configReader) 
	 : Module(x_configReader), m_param(x_configReader)
{
	// Init input streams
	AddInputStream(0, new StreamObject("input",      m_objectsIn, *this,     "Incoming objects"));
}

LogObjects::~LogObjects(void)
{
	// delete(m_input);
	if(m_outputFile.is_open())
		m_outputFile.close();
}

void LogObjects::Reset()
{
	Module::Reset();

	m_fileName = Manager::OutputDir() + "/" + m_param.file + ".txt";
	if(m_outputFile.is_open())
		m_outputFile.close();
	m_outputFile.open (m_fileName.c_str(), fstream::app|fstream::out);
	// m_outputFile<<"time"<<SEP<<"object"<<SEP<<"feature"<<SEP<<"value"<<SEP<<"mean"<<SEP<<"sqVariance"<<SEP<<"initial"<<SEP<<"min"<<SEP<<"max"<<SEP<<"nbSamples"<<endl;
}

void LogObjects::ProcessFrame()
{
	/* The old way, unused
	// Log time stamp
	// m_outputFile<<"## "<<m_currentTimeStamp<<endl;

	// Log each feature of every object into a text file
	for(vector<Object>::const_iterator it1 = m_objectsIn.begin() ; it1 != m_objectsIn.end() ; it1++)
	{
		// it1->Serialize(m_outputFile, "");
		// continue;
		// m_outputFile<<it1->GetName()<<it1->GetId()<<endl;
		for(map<string, Feature>::const_iterator it2 = it1->GetFeatures().begin() ; it2 != it1->GetFeatures().end() ; it2++)
		{
			const Feature & feat = it2->second;
			m_outputFile<<m_currentTimeStamp<<SEP
				<<it1->GetName()<<it1->GetId()<<SEP
				<<it2->first<<SEP
				<<feat.value<<SEP
				<<feat.mean<<SEP
				<<feat.sqVariance<<SEP
				<<feat.initial<<SEP
				<<feat.min<<SEP
				<<feat.max<<SEP
				<<feat.nbSamples<<endl;
		}
		m_outputFile<<endl;
	}
	*/
	if(m_objectsIn.size() > 0)
	{
		m_outputFile<<"\"frame_"<<m_currentTimeStamp<<"\": ["<<endl;
		for(vector<Object>::const_iterator it1 = m_objectsIn.begin() ; it1 != m_objectsIn.end() ; ++it1)
		{
			m_outputFile<<"\""<<it1->GetName()<<it1->GetId()<<"\": ";
			m_outputFile<<it1->SerializeToString("")<<", "<<endl;
		}
		m_outputFile<<"]"<<endl;
	}
}

