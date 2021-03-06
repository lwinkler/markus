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


namespace mk {
/**
* @brief Count the input objects
*/
class ObjectsToEvent : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			AddParameter(new ParameterUInt(  "minObject"  ,        1, 0, INT_MAX, &minObjectsNb  , "Min number of objects"));
			AddParameter(new ParameterUInt(  "maxObject"  ,  INT_MAX, 0, INT_MAX, &maxObjectsNb  , "Max number of objects"));
			AddParameter(new ParameterString("eventName"  ,  "count_reached"    , &eventName     , "Name of the event"));
			AddParameter(new ParameterBool(  "onlyNew"    ,  true               , &onlyNew       , "Only raise an event if no object was present of previous frame"));
		}
		unsigned int minObjectsNb;
		unsigned int maxObjectsNb;
		std::string eventName;
		bool onlyNew;
	};

	explicit ObjectsToEvent(ParameterStructure& xr_params);
	MKCLASS("ObjectsToEvent")
	MKCATEG("Conversion")
	MKDESCR("Count objects and raise an event and state if a condition is reached")

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	void ProcessFrame() override;
	void Reset() override;

	// input
	std::vector <Object> m_objectsIn;

	// output
	Event m_event;
	bool  m_state;
};


} // namespace mk
#endif
