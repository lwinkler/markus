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
#include "StreamImage.h"

using namespace std;
using namespace cv;

log4cxx::LoggerPtr RandomEventGenerator::m_logger(log4cxx::Logger::getLogger("RandomEventGenerator"));

RandomEventGenerator::RandomEventGenerator(const ConfigReader& x_configReader): 
	Input(x_configReader),
	m_param(x_configReader),
	m_output(Size(m_param.width, m_param.height), m_param.type)  // Note: sizes will be overridden !
{
	m_description = "Generate events with random features at each step";
	m_timeStamp = TIME_STAMP_INITIAL;
	
	AddOutputStream(0, new StreamEvent("event", m_event, *this,  "Event generated"));
	AddOutputStream(1, new StreamImage("image", m_output, *this, "Test image"));

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
	m_output.setTo(0);

	// Generate an event with an associated object
	if(m_param.nbFeatures == 0)
	{
		m_event.Raise("random");
	}
	else
	{
		Object obj("test");
		for(int i = 0 ; i < m_param.nbFeatures ; i++)
		{
			stringstream name;
			name<<"feat"<<i;
			obj.AddFeature(name.str(), static_cast<float>(rand()) / RAND_MAX);
		}
		m_event.Raise("random", obj);

		// Output an image in relation with the event
		int x = m_param.width / 2, y = m_param.width / 2, r = m_param.width / 4,  c = 0, l = -1;
		if(m_param.nbFeatures > 0)
			x = obj.GetFeature("feat0").value * m_param.width;
		if(m_param.nbFeatures > 1)
			y = obj.GetFeature("feat1").value * m_param.height;
		if(m_param.nbFeatures > 2)
			r = obj.GetFeature("feat2").value * m_param.width / 4;
		if(m_param.nbFeatures > 3)
			c = obj.GetFeature("feat3").value * 255;
		if(m_param.nbFeatures > 4)
			l = obj.GetFeature("feat4").value * 5 + 1;
		circle(m_output, Point(x, y), r, Scalar(100, c, 255 - c), l);

		LOG_DEBUG(m_logger, "RandomEventGenerator: Capture time: "<<m_frameTimer.GetMSecLong());
		m_currentTimeStamp = m_frameTimer.GetMSecLong();
		// SetTimeStampToOutputs(m_frameTimer.GetMSecLong());
	}
}

