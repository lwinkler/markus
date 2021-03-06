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
#include "FeatureFloatInTime.h"

namespace mk {
using namespace std;
using namespace cv;

log4cxx::LoggerPtr FilterObjects::m_logger(log4cxx::Logger::getLogger("FilterObjects"));

FilterObjects::FilterObjects(ParameterStructure& xr_params) :
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params))
{
	AddInputStream(0, new StreamObject("image", 	m_objectsIn, *this,	"Incoming objects",
									   R"({"features":{
									   "x":{"type":"FeatureFloatInTime"},
									   "y":{"type":"FeatureFloatInTime"},
									   "width":{"type":"FeatureFloat"},
									   "height":{"type":"FeatureFloat"}}})"_json));

	AddOutputStream(0, new StreamObject("filtered",  m_objectsOut, *this,	"Filtered objects objects"));
#ifdef MARKUS_DEBUG_STREAMS
	// add a debug stream
	m_debug = Mat(Size(m_param.width, m_param.height), CV_8UC3);
	AddDebugStream(0, new StreamDebug("debug", m_debug, *this,	"Debug"));
#endif
}

FilterObjects::~FilterObjects()
{
}

void FilterObjects::Reset()
{
	Module::Reset();
}

void FilterObjects::ProcessFrame()
{
	//compute each object to find point of interest
#ifdef MARKUS_DEBUG_STREAMS
	static const Scalar Green = Scalar(0, 255, 33);
	static const Scalar Gray  = Scalar(200, 200, 200);
	m_debug.setTo(0);
#endif
	// Filter incoming objects and add them to the output
	m_objectsOut.clear();

	double sqDist = pow(m_param.minTravelDist, 2);
	const double diagonal = sqrt(m_param.width * m_param.width + m_param.height * m_param.height);
	for(const auto& elem : m_objectsIn)
	{
		bool valid = true;
		const Rect &rect(elem.GetRect());
		const FeatureFloat& width  = dynamic_cast<const FeatureFloat&>(elem.GetFeature("width"));
		const FeatureFloat& height = dynamic_cast<const FeatureFloat&>(elem.GetFeature("height"));
		// const Feature& distance = elem.GetFeatureByName("distance", featureNames);

		if(	(m_param.minObjectWidth > 0 && width.value < m_param.minObjectWidth) ||
				(m_param.maxObjectWidth < 1 && width.value > m_param.maxObjectWidth) ||
				(m_param.minObjectHeight > 0 && height.value < m_param.minObjectHeight) ||
				(m_param.maxObjectHeight < 1 && height.value > m_param.maxObjectHeight))
		{
			valid = false;
		}

		const FeatureFloatInTime* posX = nullptr;
		const FeatureFloatInTime* posY = nullptr;
		// cout<<POW2(posX.value - posX.initial) + POW2(posY.value - posY.initial)<<" >= "<<POW2(m_param.minDist)<<endl;
		if(sqDist > 0)
		{
			posX = dynamic_cast<const FeatureFloatInTime*>(&elem.GetFeature("x"));
			posY = dynamic_cast<const FeatureFloatInTime*>(&elem.GetFeature("y"));
			if(posX == nullptr || posY == nullptr)
				throw MkException("Can only compute distance if the object is tracked. A tracker must be present uphill.", LOC);
			if(pow(posX->value - posX->initial, 2) + pow(posY->value - posY->initial, 2) < sqDist)
			{
				valid = false;
			}
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

		if(!m_param.customFeature.empty())
		{
			try
			{
				const FeatureFloat& custom = dynamic_cast<const FeatureFloat&>(elem.GetFeature(m_param.customFeature));

				if(	(m_param.minCustom > 0 && custom.value < m_param.minCustom) ||
						(m_param.maxObjectWidth < FLT_MAX && width.value > m_param.maxCustom))
				{
					valid = false;
				}
			}
			catch(MkException& e)
			{
				LOG_DEBUG(m_logger, "Feature " << m_param.customFeature << " is unexistant: " << e.what());
				valid = false;
			}
		}

		// Extract a custom feature and test value
		if(valid)
			m_objectsOut.push_back(elem);
#ifdef MARKUS_DEBUG_STREAMS
		rectangle(m_debug, rect, valid ? Green : Gray, 1, 8);
		// note: we recompute since this may or may not have been done
		posX = dynamic_cast<const FeatureFloatInTime*>(&elem.GetFeature("x"));
		posY = dynamic_cast<const FeatureFloatInTime*>(&elem.GetFeature("y"));
		if(posX != nullptr && posY != nullptr)
			line(m_debug, Point(posX->initial * diagonal, posY->initial * diagonal), Point(posX->value * diagonal, posY->value * diagonal), valid ? Green : Gray, 1, 8);
#else
		(void)posX;
		(void)posY;
#endif
	}
	// Max number of objects criterion
	if(m_param.maxObjectsNb >= 0 && m_objectsOut.size() > static_cast<unsigned int>(m_param.maxObjectsNb))
		m_objectsOut.clear();


}

} // namespace mk
