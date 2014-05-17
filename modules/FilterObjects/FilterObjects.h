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

class ConfigReader;

class FilterObjectsParameterStructure : public ModuleParameterStructure
{
public:
	FilterObjectsParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterDouble("min_travel_dist", 0.0, 0, 1,    &minTravelDist  , "An object must have been tracked on this distance to be accepted [% of image diagonal]"));
		m_list.push_back(new ParameterDouble("min_border_dist", 0.0, 0, 1,    &minBorderDist  , "An object must be distant from the image border [% of image diagonal]"));
		m_list.push_back(new ParameterInt(   "max_object_nb"  ,  -1, -1, 1000, &maxObjectsNb   , "If there are more than this number of objects, all are filtered out"));
		m_list.push_back(new ParameterDouble("min_obj_width"  , 0.0, 0, 1,    &minObjectWidth , "Minimum width to accept an object"));
		m_list.push_back(new ParameterDouble("min_obj_height" , 0.0, 0, 1,    &minObjectHeight, "Minimum hdight to accept an object"));
		m_list.push_back(new ParameterDouble("max_obj_width"  , 1.0, 0, 1,    &maxObjectWidth , "Maximum width to accept an object"));
		m_list.push_back(new ParameterDouble("max_obj_height" , 1.0, 0, 1,    &maxObjectHeight, "Maximum hdight to accept an object"));

		Init();
	}
	double minTravelDist;
	double minBorderDist;
	double minObjectWidth;
	double minObjectHeight;
	double maxObjectWidth;
	double maxObjectHeight;
	int    maxObjectsNb;
};

/**
* @brief Filter the input objects based on different criterion
*/
class FilterObjects : public Module
{
public:
	FilterObjects(const ConfigReader& x_configReader);
	~FilterObjects();
	MKCLASS("FilterObjects")
	MKDESCR("Filter the input objects based on different criterion")
	
	virtual void ProcessFrame();
	void Reset();
private:
	FilterObjectsParameterStructure m_param;
	inline virtual const ModuleParameterStructure& GetParameters() const { return m_param;}
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


#endif
