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
		m_list.push_back(new ParameterDouble(0, "min_distance", 0.0, PARAM_DOUBLE, 0, 100000, &minDist, "Minimal distance criterion for the object"));

		ParameterStructure::Init();
	}
	double minDist;
};

class FilterObjects : public Module
{
public:
	FilterObjects(const ConfigReader& x_configReader);
	~FilterObjects();
	
	virtual void ProcessFrame();
	void Reset();
	void AddFeatureNames();
	
private:
	FilterObjectsParameterStructure m_param;

	std::vector <Object> m_objectsIn;
	std::vector <Object> m_objectsOut;
	StreamObject* m_inputObjectStream;
	StreamObject* m_outputObjectStream;

	static const char * m_type;

protected:
	inline virtual ModuleParameterStructure& RefParameter() { return m_param;}
};


#endif
