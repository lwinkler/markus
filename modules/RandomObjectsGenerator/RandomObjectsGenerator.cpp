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

#include "RandomObjectsGenerator.h"
#include "StreamObject.h"

#include <unistd.h>

using namespace std;
using namespace cv;

log4cxx::LoggerPtr RandomObjectsGenerator::m_logger(log4cxx::Logger::getLogger("RandomObjectsGenerator"));

RandomObjectsGenerator::RandomObjectsGenerator(const ConfigReader& x_configReader): 
	Input(x_configReader),
	m_param(x_configReader)
{
	AddOutputStream(0, new StreamObject("objects", m_objects, *this,  "Objects generated"));
	m_seed = 0;
	m_cpt = 0;
}

RandomObjectsGenerator::~RandomObjectsGenerator()
{
}

void RandomObjectsGenerator::Reset()
{
	Module::Reset();

	if(m_param.randomSeed == 0)
		m_seed = time(NULL);
	else m_seed = m_param.randomSeed;
	m_cpt = 0;

	m_objects.clear();
	for(int i = 0 ; i < m_param.nbObjects ; i++)
	{
		// Add an object to track
		m_objects.push_back(Object("test"));
		m_objects.back().AddFeature("gt_id",  new FeatureInt(m_cpt));
		m_objects.back().AddFeature("x",      new FeatureFloat(static_cast<float>(rand_r(&m_seed)) / RAND_MAX / 2.0));
		m_objects.back().AddFeature("y",      new FeatureFloat(static_cast<float>(rand_r(&m_seed)) / RAND_MAX / 2.0));
		m_objects.back().AddFeature("width",  new FeatureFloat(static_cast<float>(rand_r(&m_seed)) / RAND_MAX / 2.0));
		m_objects.back().AddFeature("height", new FeatureFloat(static_cast<float>(rand_r(&m_seed)) / RAND_MAX / 2.0));
		m_cpt++;
	}
	// We must initialize the last time stamp
	m_lastTimeStamp = - 1000 / m_param.fps;
}

void RandomObjectsGenerator::Capture()
{
	// Wait to act consistently with other inputs
	if(m_realTime)
		usleep(1000000 / m_param.fps);
	
	const double diagonal = sqrt(m_param.width * m_param.width + m_param.height * m_param.height);
	int i = rand_r(&m_seed) % 100;

	if(i % 10 == 0)
	{
		// Add an object to track
		m_objects.push_back(Object("test"));
		m_objects.back().AddFeature("gt_id",  new FeatureInt(m_cpt));
		m_objects.back().AddFeature("x",      new FeatureFloat(static_cast<float>(rand_r(&m_seed)) / RAND_MAX / 2.0));
		m_objects.back().AddFeature("y",      new FeatureFloat(static_cast<float>(rand_r(&m_seed)) / RAND_MAX / 2.0));
		m_objects.back().AddFeature("width",  new FeatureFloat(static_cast<float>(rand_r(&m_seed)) / RAND_MAX / 2.0));
		m_objects.back().AddFeature("height", new FeatureFloat(static_cast<float>(rand_r(&m_seed)) / RAND_MAX / 2.0));
		m_cpt++;
	}

	if(i % 10 == 1)
	{
		// Remove an object
		if(!m_objects.empty())
			m_objects.erase (m_objects.begin());
	}

	// Add random changes to features
	for(vector<Object>::iterator it = m_objects.begin() ; it != m_objects.end() ; ++it)
	{
		it->AddFeature("x",      dynamic_cast<const FeatureFloat&>(it->GetFeature("x")).value      + (static_cast<float>(rand_r(&m_seed)) / RAND_MAX - 0.5) * m_param.speed);
		it->AddFeature("y",      dynamic_cast<const FeatureFloat&>(it->GetFeature("y")).value      + (static_cast<float>(rand_r(&m_seed)) / RAND_MAX - 0.5) * m_param.speed);
		it->AddFeature("width",  dynamic_cast<const FeatureFloat&>(it->GetFeature("width")).value  + (static_cast<float>(rand_r(&m_seed)) / RAND_MAX - 0.5) * m_param.speed);
		it->AddFeature("height", dynamic_cast<const FeatureFloat&>(it->GetFeature("height")).value + (static_cast<float>(rand_r(&m_seed)) / RAND_MAX - 0.5) * m_param.speed);

		it->posX   = dynamic_cast<const FeatureFloat&>(it->GetFeature("x")).value * diagonal;
		it->posY   = dynamic_cast<const FeatureFloat&>(it->GetFeature("y")).value * diagonal;
		it->width  = dynamic_cast<const FeatureFloat&>(it->GetFeature("width")).value * diagonal;
		it->height = dynamic_cast<const FeatureFloat&>(it->GetFeature("height")).value * diagonal;
	}
	m_currentTimeStamp = m_lastTimeStamp + 1000 / m_param.fps;
	LOG_DEBUG(m_logger, "RandomObjectsGenerator: Capture time: "<<m_currentTimeStamp);
}

