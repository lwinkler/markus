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
#include "Manager.h"

using namespace std;
using namespace cv;


StreamEvent::StreamEvent(const string& x_name, Event& x_event, Module& rx_module, const string& rx_description) :
	Stream(x_name, rx_module, rx_description),
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

	if(! m_event.IsRaised()) return;

	Object& obj(m_event.RefObject());
	double ratioX = static_cast<double>(m_width) / pstream->GetInputWidth();
	double ratioY = static_cast<double>(m_height) / pstream->GetInputHeight();
	obj.posX   *= ratioX;
	obj.posY   *= ratioY;
	obj.width  *= ratioX;
	obj.height *= ratioY;
}

/// Render : to display the event

void StreamEvent::RenderTo(Mat& x_output) const
{
	if(m_event.IsRaised())
	{
		x_output.setTo(Scalar(255, 255, 255));
		m_event.RefObject().RenderTo(x_output, Scalar(255, 0, 0));
	}
	else x_output.setTo(0);
}

/// Write the stream content to a directory
void StreamEvent::WriteToDirectory(const std::string x_directory) const
{
	ofstream of;
	string fileName = x_directory + "/" + GetModule().GetName() + "." + GetName() + ".event.txt";
	of.open(fileName.c_str());
	m_event.Serialize(of);
	of.close();
}
