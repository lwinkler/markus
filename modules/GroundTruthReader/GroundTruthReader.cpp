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

#include "GroundTruthReader.h"
#include "StreamImage.h"
#include "StreamState.h"
#include "StreamDebug.h"
#include "util.h"

using namespace std;
using namespace cv;

log4cxx::LoggerPtr GroundTruthReader::m_logger(log4cxx::Logger::getLogger("GroundTruthReader"));


GroundTruthReader::GroundTruthReader(const ConfigReader& x_configReader): 
	Module(x_configReader),
	m_param(x_configReader),
	m_input(Size(m_param.width, m_param.height), m_param.type)
{
	mp_annotationReader = NULL;
	AddInputStream(0, new StreamImage("input",  m_input, *this, "Video input"));

	// AddOutputStream(0, new StreamImage("input", m_input,  *this, 	"Copy of the input stream"));
	AddOutputStream(0, new StreamState("state", m_state,  *this, 	"State read from the annotation file"));

#ifdef MARKUS_DEBUG_STREAMS
	m_debug = Mat(Size(m_param.width, m_param.height), CV_8UC3);
	AddDebugStream(0, new StreamDebug("debug", m_debug,  *this, 	"Debug stream"));
#endif
}

GroundTruthReader::~GroundTruthReader()
{
	CLEAN_DELETE(mp_annotationReader);
}

void GroundTruthReader::Reset()
{
	Module::Reset();
	m_state    = false;

	CLEAN_DELETE(mp_annotationReader);
	mp_annotationReader = new AnnotationFileReader();
	mp_annotationReader->Open(m_param.file);
}

void GroundTruthReader::ProcessFrame()
{
	m_state = false;

	// Interpret the current state
	string text = mp_annotationReader->ReadAnnotationForTimeStamp(m_currentTimeStamp);
	LOG_DEBUG(m_logger, "Read annotation \"" + text + "\" for time stamp "<<m_currentTimeStamp);
	if(text != "")
	{
		m_state = text.find(m_param.pattern) != string::npos; // TODO: Improve this with regexp
#ifdef MARKUS_DEBUG_STREAMS
		m_debug.setTo(0);
		Scalar color = m_state ? Scalar(0, 255, 0) : Scalar(0, 0, 255); 
		putText(m_debug, text, Point(40, 40),  FONT_HERSHEY_COMPLEX_SMALL, 0.8, color);
#endif
	}
	else
	{
		m_state = false;
#ifdef MARKUS_DEBUG_STREAMS
		m_debug.setTo(0);
#endif
	}
}


