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

#include "RandomEventGenerator.h"
// #include "util.h"
#include "StreamEvent.h"

using namespace std;

RandomEventGenerator::RandomEventGenerator(const ConfigReader& x_configReader): 
	Input(x_configReader),
	m_param(x_configReader)
{
	m_timeStamp = TIME_STAMP_INITIAL;
	
	m_outputStreams.push_back(new StreamEvent(0, "event", m_param.width, m_param.height, m_event, *this, 		"Event generated"));

	if(m_param.randomSeed == 0)
		srand(time(NULL));
	else srand(m_param.randomSeed);
}

RandomEventGenerator::~RandomEventGenerator()
{
}

void RandomEventGenerator::Reset()
{
	Module::Reset();
	m_event.Empty();
	m_frameTimer.Restart();
}

void RandomEventGenerator::Capture()
{
	m_event.Empty();
	Object obj("test");
	for(int i = 0 ; i < m_param.nbFeatures ; i++)
	{
		stringstream name;
		name<<"feat"<<i;
		obj.AddFeature(name.str(), static_cast<float>(rand()) / RAND_MAX);
	}
	m_event.Raise("random", obj);
	LOG_DEBUG(m_logger, "RandomEventGenerator: Capture time: "<<m_frameTimer.GetMSecLong());
	SetTimeStampToOutputs(m_frameTimer.GetMSecLong());
}

