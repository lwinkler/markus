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
// #include "util.h"
// #define POW2(x) (x) * (x)

using namespace std;
using namespace cv;


FilterObjects::FilterObjects(const ConfigReader& x_configReader) :
	Module(x_configReader),
	m_param(x_configReader)
{
	m_description = "Filter the input objects based on different criterion";

	AddInputStream(0, new StreamObject("input", 	m_objectsIn, *this,	"Incoming objects"));

	AddOutputStream(0, new StreamObject("filtered",  m_objectsOut, *this,	"Filtered objects objects"));
#ifdef MARKUS_DEBUG_STREAMS
	// add a debug stream
	m_debug = Mat(Size(m_param.width, m_param.height), CV_8UC3);
	AddOutputStream(0, new StreamDebug("debug", m_debug, *this,	"Debug"));
#endif
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
#ifdef MARKUS_DEBUG_STREAMS
	static const Scalar Green = Scalar(0, 255, 33);
	static const Scalar Gray  = Scalar(200, 200, 200);
	m_debug.setTo(0);
#endif
	// Filter incoming objects and add them to the output
	m_objectsOut.clear();

	double sqDist = pow(m_param.minTravelDist, 2);
	const double diagonal = sqrt(m_param.width * m_param.width + m_param.height * m_param.height);
	for(vector<Object>::const_iterator it = m_objectsIn.begin() ; it != m_objectsIn.end() ; it++)
	{
		bool valid = true;
		const Rect &rect(it->Rect());
		const Feature& posX   = it->GetFeature("x");
		const Feature& posY   = it->GetFeature("y");
		const Feature& width  = it->GetFeature("width");
		const Feature& height = it->GetFeature("height");
		// const Feature& distance = it->GetFeatureByName("distance", featureNames);

		if(	(m_param.minObjectWidth > 0 && width.value < m_param.minObjectWidth) ||
			(m_param.maxObjectWidth < 1 && width.value > m_param.maxObjectWidth) ||
			(m_param.minObjectHeight > 0 && height.value < m_param.minObjectHeight) ||
			(m_param.maxObjectHeight < 1 && height.value > m_param.maxObjectHeight))
		{
			valid = false;
		}

		// cout<<POW2(posX.value - posX.initial) + POW2(posY.value - posY.initial)<<" >= "<<POW2(m_param.minDist)<<endl;
		if(pow(posX.value - posX.initial, 2) + pow(posY.value - posY.initial, 2) < sqDist)
		{
			valid = false;
		}
		// If the object touches the border, do not valid
		//	value is set to 2 = 1 + 1 so that we avoid rounding errors
		if(m_param.minBorderDist > 0)
		{
			const double dist = m_param.minBorderDist * diagonal;
			if(rect.x <= dist || rect.y <= dist 
				|| m_param.width - rect.x - rect.width <= dist || m_param.height - rect.y - rect.height <= dist)
			{
				valid = false;
			}
		}
		if(valid)
			m_objectsOut.push_back(*it);
#ifdef MARKUS_DEBUG_STREAMS
		rectangle(m_debug, rect, valid ? Green : Gray, 1, 8);
		line(m_debug, Point(posX.initial * diagonal, posY.initial * diagonal), Point(posX.value * diagonal, posY.value * diagonal), valid ? Green : Gray, 1, 8);
#endif
	}
	// Max number of objects criterion
	if(m_param.maxObjectsNb >= 0 && m_objectsOut.size() > static_cast<unsigned int>(m_param.maxObjectsNb))
		m_objectsOut.clear();

}

