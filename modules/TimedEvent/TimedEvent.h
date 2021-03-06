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

#ifndef TIMED_EVENT_H
#define TIMED_EVENT_H

#include "Module.h"
#include "Parameter.h"
#include "Event.h"

namespace mk {
/**
* @brief Detect motion from an image where pixel value represents motion
*/
class TimedEvent : public Module
{
public:
	class Parameters : public Module::Parameters
	{

	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			AddParameter(new ParameterDouble("timeInterval", 30, 0.001, 3600 * 24 * 365, &timeInterval, "Time interval in seconds"));
		}
		double timeInterval;
	};

	explicit TimedEvent(ParameterStructure& xr_params);
	~TimedEvent() override;
	MKCLASS("TimedEvent")
	MKCATEG("EventDetector")
	MKDESCR("Trigger an event periodically (at a specified interval of time)")

	bool IsInputProcessed() const override;
private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	void ProcessFrame() override;
	void Reset() override;

	// input
	cv::Mat m_input;

	// output
	Event  m_event;

	// temp
	TIME_STAMP m_nextEvent;

	// debug
#ifdef MARKUS_DEBUG_STREAMS
	cv::Mat m_debug;
#endif
};

} // namespace mk
#endif

