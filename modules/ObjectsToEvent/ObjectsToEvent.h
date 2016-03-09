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

#ifndef OBJECTS_TO_EVENTS
#define OBJECTS_TO_EVENTS

#include "Module.h"
#include "StreamObject.h"
#include "StreamEvent.h"


/**
* @brief Count the input objects
*/
class ObjectsToEvent : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterUInt(  "min_object"  ,        1, 0, INT_MAX, &minObjectsNb  , "Min number of objects"));
			m_list.push_back(new ParameterUInt(  "max_object"  ,  INT_MAX, 0, INT_MAX, &maxObjectsNb  , "Max number of objects"));
			m_list.push_back(new ParameterString("event_name"  ,  "count_reached"    , &eventName     , "Name of the event"));
			m_list.push_back(new ParameterBool(  "only_new"    ,        1, 0,       1, &onlyNew       , "Only raise an event if no object was present of previous frame"));

			Init();
		}
		unsigned int minObjectsNb;
		unsigned int maxObjectsNb;
		std::string eventName;
		bool onlyNew;
	};

	ObjectsToEvent(ParameterStructure& xr_params);
	MKCLASS("ObjectsToEvent")
	MKCATEG("Conversion")
	MKDESCR("Count objects and raise an event and state if a condition is reached")

	virtual void ProcessFrame() override;
	virtual void Reset() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:

	// input
	std::vector <Object> m_objectsIn;

	// output
	Event m_event;
	bool  m_state;
};


#endif
