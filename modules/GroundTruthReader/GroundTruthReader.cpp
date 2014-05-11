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


/**
* @brief Getline which is safe for all line ends
*
* @param is Input stream
* @param t  Output string
*
* @return 
*/
std::istream& safeGetline(std::istream& is, std::string& t)
{
	t.clear();

	// The characters in the stream are read one-by-one using a std::streambuf.
	// That is faster than reading them one-by-one using the std::istream.
	// Code that uses streambuf this way must be guarded by a sentry object.
	// The sentry object performs various tasks,
	// such as thread synchronization and updating the stream state.

	std::istream::sentry se(is, true);
	std::streambuf* sb = is.rdbuf();

	for(;;) {
		int c = sb->sbumpc();
		switch (c) {
			case '\n':
				return is;
			case '\r':
				if(sb->sgetc() == '\n')
					sb->sbumpc();
				return is;
			case EOF:
				// Also handle the case when the last line has no line ending
				if(t.empty())
					is.setstate(std::ios::eofbit);
				return is;
			default:
				t += (char)c;
		}
	}
}

GroundTruthReader::GroundTruthReader(const ConfigReader& x_configReader): 
	Module(x_configReader),
	m_param(x_configReader),
	m_input(Size(m_param.width, m_param.height), m_param.type)
{
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
	m_srtFile.close();
}

void GroundTruthReader::Reset()
{
	Module::Reset();
	m_state    = false;
	m_stateSub = false;
	m_srtStart = msToTimeStamp(0);
	m_srtEnd   = msToTimeStamp(0);
	m_num      = -1;
	m_subText  = "";

	LOG_DEBUG(m_logger, "Open ground truth file: "<<m_param.file);

	if(m_srtFile.is_open())
		m_srtFile.close();
	m_srtFile.open(m_param.file.c_str(), std::ifstream::in);
	
	if(! m_srtFile.is_open())
	{
		throw MkException("Error : GroundTruthReader cannot open file : " + m_param.file, LOC);
	}
}

void GroundTruthReader::ReadNextSubtitle()
{
	string line, tmp;
	int num = -1;
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
				throw MkException("End of file in GroundTruthReader", LOC);
		}
		num = atoi(line.c_str());
		LOG_DEBUG(m_logger, "Subtitle nb "<<num);
		if(num != m_num + 1)
			LOG_WARN(m_logger, "Missing subtitle number "<<(m_num + 1));
		m_num = num;
		//safeGetline(m_srtFile, line);
		//istringstream ss(line);
		//ss >> m_srtStart >> tmp >> m_srtEnd;
		m_srtFile >> m_srtStart;
		m_srtFile >> tmp;
		if(tmp != "-->")
			throw MkException("Subtitle format error: must contain '-->'", LOC);
		m_srtFile >> m_srtEnd;

		safeGetline(m_srtFile, line); // end of line: must be empty
		if(! line.empty())
			throw MkException("Subtitle format error. There must be an empty line after subtitle.", LOC);
		safeGetline(m_srtFile, line);

		m_subText = "";
		while(line.size() != 0)
		{
			m_subText += line + " ";
			safeGetline(m_srtFile, line);
			if(! m_srtFile.good())
				throw MkException("End of file in GroundTruthReader", LOC);
		}
		LOG_DEBUG(m_logger, "Read next sub: "<<m_subText);
		m_stateSub = m_subText.find(m_param.pattern) != string::npos; // TODO: Improve this with regexp
	}
	catch(MkException& e)
	{
		LOG_ERROR(m_logger, "Exception while reading .srt file in GroundTruthReader: " << e.what()); 
		throw;
	}
}

void GroundTruthReader::ProcessFrame()
{
	if(! m_srtFile.good())
	{
		m_state = false;
		return;
	}
	

	// istringstream ss;
	m_state = 0;
	string current = msToTimeStamp(m_currentTimeStamp);

	while(current > m_srtEnd)
	{
		ReadNextSubtitle();
	}


	// Interpret the current state
	if(current >= m_srtStart && current <= m_srtEnd)
	{
		m_state = m_stateSub;
#ifdef MARKUS_DEBUG_STREAMS
		m_debug.setTo(0);
		Scalar color = m_stateSub ? Scalar(0, 255, 0) : Scalar(0, 0, 255); 
		putText(m_debug, m_subText, Point(40, 40),  FONT_HERSHEY_COMPLEX_SMALL, 0.8, color);
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


