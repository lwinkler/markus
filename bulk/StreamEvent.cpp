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

#include "StreamEvent.h"
#include "util.h"

using namespace std;
using namespace cv;


StreamEvent::StreamEvent(int x_id, const string& x_name, Event& x_event, Module& rx_module, const string& rx_description) : 
	Stream(x_id, x_name, STREAM_EVENT, 1, 1, rx_module, rx_description),
	m_event(x_event)
{
}


StreamEvent::~StreamEvent()
{

}

// Transmit the event to the connected module

void StreamEvent::ConvertInput()
{
	if(m_connected == NULL) return;
	
	// Copy time stamp to output
	m_timeStamp = RefConnected().GetTimeStamp();

	const StreamEvent * pstream = dynamic_cast<const StreamEvent*>(m_connected);
	if(pstream == NULL) return;
	m_event = pstream->GetEvent();

	Object obj = pstream->m_event.GetObject();
	double ratioX = static_cast<double>(m_width) / pstream->GetInputWidth();
	double ratioY = static_cast<double>(m_height) / pstream->GetInputHeight();
	obj.m_posX   *= ratioX;
	obj.m_posY   *= ratioY;
	obj.m_width  *= ratioX;
	obj.m_height *= ratioY;
	pstream->m_event.SetObject(obj);
}

/// Render : to display the event

void StreamEvent::RenderTo(Mat * xp_output) const
{
	xp_output->setTo(Scalar(255 * m_event.IsRaised(), 255 * m_event.IsRaised(), 255 * m_event.IsRaised())); // TODO : this should be improved
}

