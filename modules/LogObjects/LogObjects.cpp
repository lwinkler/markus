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

#include "LogObjects.h"
#include "StreamState.h"
#include "util.h"
#include "feature_util.h"
#include "Manager.h"

//#include <ctime>

#define SEP "\t"

using namespace std;
using namespace cv;

log4cxx::LoggerPtr LogObjects::m_logger(log4cxx::Logger::getLogger("LogObjects"));

LogObjects::LogObjects(ParameterStructure& xr_params)
	: Module(xr_params), m_param(dynamic_cast<Parameters&>(xr_params))
{
	// Init input streams
	AddInputStream(0, new StreamObject("input",      m_objectsIn, *this,     "Incoming objects"));
	mp_annotationWriter = nullptr;
}

LogObjects::~LogObjects(void)
{
	CLEAN_DELETE(mp_annotationWriter);
}

void LogObjects::Reset()
{
	Module::Reset();

	string srtFileName = m_context.GetOutputDir() + "/" + m_param.file;
	CLEAN_DELETE(mp_annotationWriter);
	mp_annotationWriter = new AnnotationFileWriter();
	mp_annotationWriter->Open(srtFileName);

	// m_folder  = m_context.GetOutputDir() + "/" + m_param.folder + "/";
	// SYSTEM("mkdir -p " + m_folder);
	// m_outputFile<<"time"<<SEP<<"object"<<SEP<<"feature"<<SEP<<"value"<<SEP<<"mean"<<SEP<<"sqVariance"<<SEP<<"initial"<<SEP<<"min"<<SEP<<"max"<<SEP<<"nbSamples"<<endl;
}

void LogObjects::ProcessFrame()
{
	LOG_DEBUG(m_logger, "Write object to log file");
	stringstream ss;
	serialize(ss, m_objectsIn);
	mp_annotationWriter->WriteAnnotation(m_currentTimeStamp, m_currentTimeStamp, ss);
}

