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

#ifndef EVENT_TO_OBJECTS_H
#define EVENT_TO_OBJECTS_H

#include "Module.h"
#include "Event.h"
#include "Object.h"


/**
* @brief Read an event and log it to .srt file
*/
class EventToObjects : public Module
{
	class Parameters : public ModuleParameterStructure
	{
		public:
			Parameters(const ConfigReader& x_confReader) : 
				ModuleParameterStructure(x_confReader)
		{
			Init();
		}
	};

public:
	EventToObjects(const ConfigReader& x_configReader);
	~EventToObjects(void);
	MKCLASS("EventToObjects")
	MKDESCR("Transform a stream of events into a stream of objects")

	virtual void ProcessFrame();
	void Reset();

private:
	inline virtual const Parameters& GetParameters() const { return m_param;}
	Parameters m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	virtual bool IsInputProcessed() const;

	// input
	Event m_event;

	// output
	std::vector<Object> m_objects;
};

#endif

