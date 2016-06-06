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

#include "AnnotationSrtFileReader.h"
#include "StreamImage.h"
#include "StreamState.h"
#include "StreamDebug.h"
#include "util.h"

using namespace std;
using namespace cv;

log4cxx::LoggerPtr AnnotationSrtFileReader::m_logger(log4cxx::Logger::getLogger("AnnotationSrtFileReader"));


AnnotationSrtFileReader::AnnotationSrtFileReader()
{
}

AnnotationSrtFileReader::~AnnotationSrtFileReader()
{
}

bool AnnotationSrtFileReader::ReadNextAnnotation(string& rx_subText)
{
	if(! m_srtFile.good())
	{
		LOG_DEBUG(m_logger, "End of subtitle file");
		rx_subText = "";
		return false;
	}

	string line, tmp;
	try
	{
		// Read next subtitle
		// 2
		// 00:00:30,700 --> 00:00:31,900
		// state_0

		while(line.size() == 0)
		{
			m_srtFile >> line;
			if(! m_srtFile.good())
				throw EndOfStreamException("End of file ", LOC);
		}
		int num = boost::lexical_cast<int>(line);
		LOG_DEBUG(m_logger, "Subtitle nb "<<num);
		if(num != m_num + 1)
			LOG_WARN(m_logger, "Missing subtitle number "<<(m_num + 1));
		m_num = num;
		//SafeGetline(m_srtFile, line);
		//istringstream ss(line);
		//ss >> m_srtStart >> tmp >> m_srtEnd;
		m_srtFile >> m_srtStart;
		m_srtFile >> tmp;
		if(tmp != "-->")
			throw MkException("Subtitle format error: must contain '-->'", LOC);
		m_srtFile >> m_srtEnd;

		SafeGetline(m_srtFile, line); // end of line: must be empty
		if(! line.empty())
			throw MkException("Subtitle format error. There must be an empty line after subtitle.", LOC);
		SafeGetline(m_srtFile, line);

		rx_subText = "";
		while(line.size() != 0)
		{
			rx_subText += line + " ";
			SafeGetline(m_srtFile, line);
			if(! m_srtFile.good())
				throw EndOfStreamException("End of file", LOC);
		}
		LOG_DEBUG(m_logger, "Read next sub: "<<rx_subText);
	}
	catch(EndOfStreamException& e)
	{
		LOG_INFO(m_logger, "End of .srt file : " << e.what());
		rx_subText = "";
		m_srtStart = msToTimeStamp(0);
		m_srtEnd   = msToTimeStamp(0);
		return false;
	}
	return true;
}


