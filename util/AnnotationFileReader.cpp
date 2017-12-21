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

#include "AnnotationFileReader.h"
#include "StreamImage.h"
#include "StreamState.h"
#include "StreamDebug.h"
#include "util.h"

namespace mk {
using namespace std;
using namespace cv;

log4cxx::LoggerPtr AnnotationFileReader::m_logger(log4cxx::Logger::getLogger("AnnotationFileReader"));


/**
* @brief Getline which is safe for all line ends
*
* @param is Input stream
* @param t  Output string
*
* @return
*/
istream& AnnotationFileReader::SafeGetline(istream& is, string& t)
{
	t.clear();

	// The characters in the stream are read one-by-one using a std::streambuf.
	// That is faster than reading them one-by-one using the std::istream.
	// Code that uses streambuf this way must be guarded by a sentry object.
	// The sentry object performs various tasks,
	// such as thread synchronization and updating the stream state.

	// cppcheck-suppress unreadVariable
	istream::sentry se(is, true);
	streambuf* sb = is.rdbuf();

	for(;;)
	{
		int c = sb->sbumpc();
		switch (c)
		{
		case '\n':
			return is;
		case '\r':
			if(sb->sgetc() == '\n')
				sb->sbumpc();
			return is;
		case EOF:
			// Also handle the case when the last line has no line ending
			if(t.empty())
				is.setstate(ios::eofbit);
			return is;
		default:
			t += static_cast<char>(c);
		}
	}
}

AnnotationFileReader::AnnotationFileReader()
{
	m_num = 0;
}

AnnotationFileReader::~AnnotationFileReader()
{
	if(m_srtFile.is_open())
		m_srtFile.close();
}

TIME_STAMP AnnotationFileReader::GetCurrentTimeStamp()
{
	return timeStampToMs(m_srtStart);
}

TIME_STAMP AnnotationFileReader::GetEndTimeStamp()
{
	return timeStampToMs(m_srtEnd);
}

void AnnotationFileReader::Open(const string& x_file)
{
	m_num      = -1;
	m_text     = "";
	m_srtStart = msToTimeStamp(0);
	m_srtEnd   = msToTimeStamp(0);

	LOG_DEBUG(m_logger, "Open annotation file: "<<x_file);

	if(m_srtFile.is_open())
		m_srtFile.close();
	m_srtFile.open(x_file.c_str(), ifstream::in);

	if(! m_srtFile.is_open())
	{
		throw MkException("Error : AnnotationFileReader cannot open file : " + x_file, LOC);
	}
}

string AnnotationFileReader::ReadAnnotationForTimeStamp(TIME_STAMP x_current)
{
	if(! m_srtFile.good())
	{
		LOG_DEBUG(m_logger, "End of subtitle file");
		return "";
	}

	string current = msToTimeStamp(x_current);

	while(current > m_srtEnd)
	{
		bool ret = ReadNextAnnotation(m_text);
		if(!ret)
			break;
	}
	// LOG_DEBUG(m_logger, "Check that time "<<current<<" is between "<<m_srtStart<<" and "<<m_srtEnd<<" "<<(current >= m_srtStart && current <= m_srtEnd));
	if(current >= m_srtStart && current <= m_srtEnd)
		return m_text;
	else return "";
}
} // namespace mk
