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

#include "FilterObjects.h"
#include "StreamObject.h"

// for debug
#include "util.h"


#define POW2(x) (x) * (x)

using namespace cv;

const char * FilterObjects::m_type = "FilterObjects";

using namespace std;
using namespace cv;


FilterObjects::FilterObjects(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader)
{
	m_description = "Filter the input objects based on different criterion";

	m_inputObjectStream = new StreamObject(0, "input", 	m_param.width, m_param.height, m_objectsIn, cvScalar(255, 255, 255), *this,	"Incoming objects");
	m_inputStreams.push_back(m_inputObjectStream);

	m_outputObjectStream = new StreamObject(0, "filtered",  m_param.width, m_param.height, m_objectsOut, cvScalar(255, 255, 255), *this,	"Filtered objects objects");
	m_outputStreams.push_back(m_outputObjectStream);
}

FilterObjects::~FilterObjects(void )
{
}

void FilterObjects::Reset()
{
	Module::Reset();
}

void FilterObjects::ProcessFrame()
{
	static bool once = false;
	if(!once)
	{
		AddFeatureNames();
		once = true;
	}

	// Filter incoming objects and add them to the output
	m_objectsOut.clear();
	for(vector<Object>::const_iterator it = m_objectsIn.begin() ; it != m_objectsIn.end() ; it++)
	{
		const Feature& posX = it->GetFeature("x");
		const Feature& posY = it->GetFeature("y");
		// const Feature& distance = it->GetFeatureByName("distance", featureNames);

		if(POW2(posX.value - posX.initial) + POW2(posY.value - posY.initial) > m_param.minDist)
		// if(distance.max >= m_param.minDist)
			m_objectsOut.push_back(*it);
	}
}

void FilterObjects::AddFeatureNames()
{
	// Add features to pass to the output
	for(vector<std::string>::const_iterator it = m_inputObjectStream->GetFeatureNames().begin() ; it != m_inputObjectStream->GetFeatureNames().end() ; it++)
	{
		if(it->size() == 0)
			continue;
		m_outputObjectStream->AddFeatureName(*it);
	}
}
