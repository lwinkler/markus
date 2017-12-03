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

#include "FeatureToValue.h"
#include "StreamObject.h"

using namespace std;
using namespace cv;

log4cxx::LoggerPtr FeatureToValue::m_logger(log4cxx::Logger::getLogger("FeatureToValue"));

FeatureToValue::FeatureToValue(ParameterStructure& xr_params) :
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_value(0)
{
	string req = "{\"features\":{\"" + m_param.feature + "\":{\"type\":\"FeatureFloat\"}}}";
	AddInputStream(0, new StreamObject("image", m_objectInput, *this, "Incoming objects", mkjson(req)));

	mp_streamValues = new StreamNum<double>("value", m_value,  *this, "Scalar representing the motion level");
	AddOutputStream(0, mp_streamValues);
}

FeatureToValue::~FeatureToValue(void )
{
}


void FeatureToValue::Reset()
{
	Module::Reset();
}

void FeatureToValue::ProcessFrame()
{
	if(m_objectInput.empty())
	{
		m_value = 0;
	}
	else
	{
		const Object* largest = nullptr;
		double maxSize = -1000;
		for(const auto& elem : m_objectInput)
		{
			double s = elem.width * elem.height;
			if(s > maxSize)
			{
				maxSize = s;
				largest = &elem;
			}
		}
		assert(largest != nullptr);
		m_value = dynamic_cast<const FeatureFloat&>(largest->GetFeature(m_param.feature)).value;
	}

#ifdef MARKUS_DEBUG_STREAMS
	mp_streamValues->Store();
#endif
}
