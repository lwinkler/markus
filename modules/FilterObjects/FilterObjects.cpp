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
#include "StreamDebug.h"

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
#ifdef MARKUS_DEBUG_STREAMS
	// add a debug stream
	m_debug = new Mat(Size(m_param.width, m_param.height), CV_8UC3);
	m_debugStreams.push_back(new StreamDebug(0, "debug", m_debug, *this,	"Debug"));
#endif
}

FilterObjects::~FilterObjects(void )
{
#ifdef MARKUS_DEBUG_STREAMS
	delete(m_debug);
#endif
}

void FilterObjects::Reset()
{
	Module::Reset();
}

void FilterObjects::ProcessFrame()
{
#ifdef MARKUS_DEBUG_STREAMS
	static const Scalar Green = Scalar(0, 255, 33);
	static const Scalar Gray  = Scalar(200, 200, 200);
	m_debug->setTo(0);
#endif
	// Filter incoming objects and add them to the output
	m_objectsOut.clear();
	double sqDist = pow(m_param.minDist, 2);
	for(vector<Object>::const_iterator it = m_objectsIn.begin() ; it != m_objectsIn.end() ; it++)
	{
		bool valid = true;
		const Rect &rect(it->Rect());
		const Feature& posX = it->GetFeature("x");
		const Feature& posY = it->GetFeature("y");
		// const Feature& distance = it->GetFeatureByName("distance", featureNames);

		// cout<<POW2(posX.value - posX.initial) + POW2(posY.value - posY.initial)<<" >= "<<POW2(m_param.minDist)<<endl;
		if(pow((posX.value - posX.initial) * m_param.width, 2) + pow((posY.value - posY.initial) * m_param.height, 2) < sqDist)
		{
			valid = false;
		}
		// If the object touches the border, do not valid
		//	value is set to 2 = 1 + 1 so that we avoid rounding errors
		if(m_param.avoidBorders && (rect.x <= 2 || rect.y <= 2 || rect.x + rect.width >= m_param.width - 2 || rect.y + rect.height >= m_param.height - 2)) // TODO: It would be cleaner to have objects pos [0:width-1]
		{
			valid = false;

		}
		if(valid)
			m_objectsOut.push_back(*it);
#ifdef MARKUS_DEBUG_STREAMS
		rectangle(*m_debug, rect, valid ? Green : Gray, 1, 8);
		line(*m_debug, Point(posX.initial * m_param.width, posY.initial * m_param.height), Point(posX.value * m_param.width, posY.value * m_param.height), valid ? Green : Gray, 1, 8);
#endif
	}
}

