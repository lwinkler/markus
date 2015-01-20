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

#include "AnnotationFileWriter.h"
#include "StreamImage.h"
#include "StreamState.h"
#include "StreamDebug.h"
#include "util.h"

using namespace std;
using namespace cv;

log4cxx::LoggerPtr AnnotationFileWriter::m_logger(log4cxx::Logger::getLogger("AnnotationFileWriter"));


AnnotationFileWriter::AnnotationFileWriter()
{
}

AnnotationFileWriter::~AnnotationFileWriter()
{
	if(m_file.is_open())
		m_file.close();
}

void AnnotationFileWriter::Open(const string& x_file)
{
	// m_srtStart = msToTimeStamp(0);
	// m_srtEnd   = msToTimeStamp(0);
	m_subId    = 0;

	LOG_DEBUG(m_logger, "Open anotation file: "<<x_file);

	if(m_file.is_open())
		m_file.close();
	m_file.open(x_file.c_str(), ios_base::app);

	if(! m_file.is_open())
	{
		throw MkException("Error : AnnotationFileWriter cannot open file : " + x_file, LOC);
	}
}


/// Write the subtitle in log file
void AnnotationFileWriter::WriteAnnotation(TIME_STAMP x_start, TIME_STAMP x_end, stringstream& x_in)
{
	string startTime = msToTimeStamp(x_start);
	string endTime   = msToTimeStamp(x_end);
	LOG_DEBUG(m_logger, "Write annotation to file");

	m_file<<m_subId<<endl;
	m_file<<startTime<<" --> "<<endTime<<endl;
	m_file<<x_in.str()<<endl;
	m_file<<endl;
	m_subId++;
}
