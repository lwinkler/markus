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

#ifndef FILTER_OBJECTS
#define FILTER_OBJECTS

#include "Module.h"
#include "StreamObject.h"


namespace mk {
/**
* @brief Filter the input objects based on different criterion
*/
class FilterObjects : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			AddParameter(new ParameterDouble("minTravelDist", 0.0, 0, 1,    &minTravelDist  , "An object must have been tracked on this distance to be accepted [% of image diagonal]"));
			AddParameter(new ParameterDouble("minBorderDist", 0.0, 0, 1,    &minBorderDist  , "An object must be distant from the image border [% of image diagonal]"));
			AddParameter(new ParameterInt(   "maxObjectNb"  ,  -1, -1, 1000, &maxObjectsNb  , "If there are more than this number of objects, all are filtered out"));
			AddParameter(new ParameterDouble("minObjWidth"  , 0.0, 0, 1,    &minObjectWidth , "Minimum width to accept an object"));
			AddParameter(new ParameterDouble("minObjHeight" , 0.0, 0, 1,    &minObjectHeight, "Minimum height to accept an object"));
			AddParameter(new ParameterDouble("maxObjWidth"  , 1.0, 0, 1,    &maxObjectWidth , "Maximum width to accept an object"));
			AddParameter(new ParameterDouble("maxObjHeight" , 1.0, 0, 1,    &maxObjectHeight, "Maximum height to accept an object"));
			AddParameter(new ParameterString("customFeature" , ""      ,    &customFeature  , "Name of a custom feature to test. Must be of type float"));
			AddParameter(new ParameterDouble("minCustom"     , 0.0     ,    &minCustom       , "Min value for the custom feature"));
			AddParameter(new ParameterDouble("maxCustom"     , DBL_MAX,     &maxCustom       , "Max value for the custom feature"));

			RefParameterByName("customFeature").SetRange(R"({"allowed":["x","y","width","height"]})"_json);
		}
		double minTravelDist;
		double minBorderDist;
		int    maxObjectsNb;
		double minObjectWidth;
		double minObjectHeight;
		double maxObjectWidth;
		double maxObjectHeight;
		std::string customFeature;
		double minCustom;
		double maxCustom;
	};

	explicit FilterObjects(ParameterStructure& xr_params);
	virtual ~FilterObjects();
	MKCLASS("FilterObjects")
	MKDESCR("Filter the input objects based on different criterion")
	MKCATEG("Other")

	void ProcessFrame() override;
	void Reset() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;
protected:

	// input
	std::vector <Object> m_objectsIn;

	// output
	std::vector <Object> m_objectsOut;

	// debug
#ifdef MARKUS_DEBUG_STREAMS
	cv::Mat m_debug;
#endif

};


} // namespace mk
#endif
