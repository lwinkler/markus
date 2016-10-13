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

#include "AddImageToEvent.h"
#include "StreamImage.h"
#include "StreamEvent.h"
#include "util.h"
#include "Manager.h"
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

log4cxx::LoggerPtr AddImageToEvent::m_logger(log4cxx::Logger::getLogger("AddImageToEvent"));

AddImageToEvent::AddImageToEvent(ParameterStructure& xr_params):
	Module(xr_params),
	m_param(dynamic_cast<Parameters&>(xr_params)),
	m_inputIm1(Size(m_param.width, m_param.height), m_param.type)
	// m_inputIm2(Size(m_param.width, m_param.height), m_param.type)
{
	AddInputStream(0, new StreamEvent("event", m_event, *this,     "Outgoing events"));
	AddInputStream(1, new StreamImage("image",  m_inputIm1, *this,   "Video input"));
	// AddInputStream(2, new StreamImage( "input2", m_inputIm2, *this,   "Binary mask"));

	AddOutputStream(0, new StreamEvent("image", m_event, *this,   "Outgoing events"));
}

AddImageToEvent::~AddImageToEvent()
{
}

void AddImageToEvent::Reset()
{
	Module::Reset();

	mp_outputDir.reset(new MkDirectory(m_param.folder, RefContext().RefOutputDir(), false));
	m_saveImage1 = m_inputStreams.at(1)->IsConnected();
}

void AddImageToEvent::ProcessFrame()
{
	if(!m_event.IsRaised())
		return;

	const Object& obj(m_event.GetObject());
	if(m_saveImage1)
	{
		std::stringstream ss1;
		ss1 << m_currentTimeStamp << "_" << m_event.GetEventName() << "_global_1." << m_param.extension;
		addExternalImage(m_inputIm1, "globalImage", mp_outputDir->ReserveFile(ss1.str()), m_event);

		if(obj.width > 0 && obj.height > 0)
		{
			std::stringstream ss2;
			ss2 << m_currentTimeStamp << "_" << m_event.GetEventName() << "_" << obj.GetName()<< obj.GetId() << "_1" << "." << m_param.extension;
			// cout<<"Save image "<<obj.m_posX<<" "<<obj.m_posY<<endl;
			addExternalImage((m_inputIm1)(obj.GetRect()), "objectImage", mp_outputDir->ReserveFile(ss2.str()), m_event);
		}
	}
}
