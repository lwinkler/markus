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

#ifndef STREAM_EVENT_H
#define STREAM_EVENT_H

#include "Stream.h"
#include "Event.h"

/// Stream in the form of located objects

class StreamEvent : public Stream
{
public:
	StreamEvent(int x_id, const std::string& rx_name, int x_width, int x_height, Event& x_event, Module& rx_module, const std::string& rx_description);
	~StreamEvent();
	inline void SetEvent(const Event& x_event){m_event = x_event;}
	inline const Event& GetEvent() const {return m_event;}
	
	virtual void ConvertInput();
	virtual void RenderTo(cv::Mat * xp_output) const;
	inline virtual const std::string GetTypeString() const {return "Event";}


protected:
	Event& m_event;

private:
	StreamEvent& operator=(const StreamEvent&);
	StreamEvent(const StreamEvent&);
};

#endif
