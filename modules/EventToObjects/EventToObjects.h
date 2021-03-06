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


namespace mk {
/**
* @brief Read an event and log it to .srt file
*/
class EventToObjects : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
		}
	};

	explicit EventToObjects(ParameterStructure& xr_params);
	~EventToObjects() override;
	MKCLASS("EventToObjects")
	MKCATEG("Conversion")
	MKDESCR("Transform a stream of events into a stream of objects")

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	void ProcessFrame() override;
	void Reset() override;
	bool IsInputProcessed() const override;

	// input
	Event m_event;

	// output
	std::vector<Object> m_objects;
};

} // namespace mk
#endif

