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

#include "ReadEvent.h"
#include "StreamEvent.h"
#include "StreamImage.h"
#include "util.h"
#include "AnnotationFileReader.h"
#include "Manager.h"


using namespace std;
using namespace cv;

log4cxx::LoggerPtr ReadEvent::m_logger(log4cxx::Logger::getLogger("ReadEvent"));

ReadEvent::ReadEvent(ParameterStructure& xr_params)
	: Input(xr_params), m_param(dynamic_cast<Parameters&>(xr_params)),
	  m_outputIm1(Size(m_param.width, m_param.height), m_param.type),
	  m_outputIm2(Size(m_param.width, m_param.height), m_param.type)
{
	// Init input images
	AddOutputStream(0, new StreamEvent("event", m_event, *this, "Input event to be logged"));
	AddOutputStream(1, new StreamImage("image", m_outputIm1, *this, "Video output for image extraction (optional)"));
	AddOutputStream(2, new StreamImage("mask" , m_outputIm2, *this, "Binary mask for image extraction (optional)"));
}

ReadEvent::~ReadEvent()
{
	CLEAN_DELETE(mp_annotationReader);
}

void ReadEvent::Reset()
{
	Input::Reset();
	m_event.Clean();

	CLEAN_DELETE(mp_annotationReader);
	mp_annotationReader = createAnnotationFileReader(m_param.file, m_param.width, m_param.height);
}

void ReadEvent::Capture()
{
	string text;
	if(!mp_annotationReader->ReadNextAnnotation(text))
	{
		m_endOfStream = true;
		throw EndOfStreamException("Cannot read next annotation", LOC);
	}
	m_currentTimeStamp = mp_annotationReader->GetCurrentTimeStamp();
	istringstream ss(text);
	m_event.Deserialize(ss, m_param.folder);
	assert(m_event.IsRaised());
}

