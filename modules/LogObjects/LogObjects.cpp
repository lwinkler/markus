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

namespace mk {
using namespace std;
using namespace cv;

log4cxx::LoggerPtr LogObjects::m_logger(log4cxx::Logger::getLogger("LogObjects"));

LogObjects::LogObjects(ParameterStructure& xr_params)
	: Module(xr_params), m_param(dynamic_cast<Parameters&>(xr_params))
{
	// Init input streams
	AddInputStream(0, new StreamObject("image",      m_objectsIn, *this,     "Incoming objects"));
	mp_annotationWriter = nullptr;
}

LogObjects::~LogObjects()
{
	Compress();
	CLEAN_DELETE(mp_annotationWriter);
}

void LogObjects::Compress()
{
	if(!m_param.compress)
		return;
	string tarFile = RefContext().RefOutputDir().ReserveFile(m_param.file + ".%d.tar.bz", m_nbReset);
	LOG_INFO(m_logger, "Compress objects to " << tarFile);
	try
	{
		// Compress file and remove
		SYSTEM("tar --remove-files -cjf " + tarFile + " -C " + RefContext().RefOutputDir().GetPath() + " " + m_param.file);
	}
	catch(MkException& e)
	{
		LOG_ERROR(m_logger, "Error while compressing objects");
	}
}

void LogObjects::Reset()
{
	Module::Reset();

	CLEAN_DELETE(mp_annotationWriter);
	mp_annotationWriter = new AnnotationFileWriter();
	mp_annotationWriter->Open(RefContext().RefOutputDir().ReserveFile(m_param.file, m_nbReset));
}

void LogObjects::ProcessFrame()
{
	LOG_DEBUG(m_logger, "Write object to log file");
	stringstream ss;
	ss << multiLine(m_objectsIn) << endl;
	mp_annotationWriter->WriteAnnotation(m_currentTimeStamp, m_currentTimeStamp, ss);
}

} // namespace mk
