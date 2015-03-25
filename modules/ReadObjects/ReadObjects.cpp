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

#include "ReadObjects.h"
#include "StreamState.h"
#include "util.h"
#include "feature_util.h"
#include "Manager.h"

#include <ctime>

#define SEP "\t"


using namespace std;
using namespace cv;

log4cxx::LoggerPtr ReadObjects::m_logger(log4cxx::Logger::getLogger("ReadObjects"));

ReadObjects::ReadObjects(ParameterStructure& xr_params)
	: Input(xr_params), m_param(dynamic_cast<Parameters&>(xr_params))
{
	// Init input streams
	AddOutputStream(0, new StreamObject("object", m_ObjectOut, *this, "Output object read from file"));
	mp_annotationReader = nullptr;
}

ReadObjects::~ReadObjects(void)
{
	// delete(m_input);
	CLEAN_DELETE(mp_annotationReader);
}

void ReadObjects::Reset()
{
	Input::Reset();
	//m_event.Empty();

	CLEAN_DELETE(mp_annotationReader);
	mp_annotationReader = new AnnotationFileReader();
	mp_annotationReader->Open(m_param.file);

	// m_outputFile<<"time"<<SEP<<"object"<<SEP<<"feature"<<SEP<<"value"<<SEP<<"mean"<<SEP<<"sqVariance"<<SEP<<"initial"<<SEP<<"min"<<SEP<<"max"<<SEP<<"nbSamples"<<endl;
}

void ReadObjects::Capture()
{
	// If needed, stop on first non-empty objects
	if(m_param.oneFrameOnly && !m_ObjectOut.empty())
		return;

	string text;

	if(!mp_annotationReader->ReadNextAnnotation(text))
	{
		m_endOfStream = true;
		Pause(true);
		throw EndOfStreamException("Cannot read next annotation", LOC);
	}
	m_currentTimeStamp = mp_annotationReader->GetCurrentTimeStamp();
	istringstream ss(text);
	// m_outputStreams.at(0)->Deserialize(ss,m_param.folder);
	deserialize(ss, m_ObjectOut);
	// LOG_DEBUG(m_logger, "Deserialized object: " << m_ObjectOut);
	//m_ObjectOut.Deserialize(ss, m_param.folder);
	//m_ObjectOut.push_back(obj);
}

