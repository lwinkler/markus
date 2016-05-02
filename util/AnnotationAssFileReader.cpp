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

#include "AnnotationAssFileReader.h"
#include "StreamImage.h"
#include "StreamState.h"
#include "StreamDebug.h"
#include "util.h"

using namespace std;
using namespace cv;

log4cxx::LoggerPtr AnnotationAssFileReader::m_logger(log4cxx::Logger::getLogger("AnnotationAssFileReader"));

const string AnnotationAssFileReader::headerStartTime = "Start";
const string AnnotationAssFileReader::headerEndTime = "End";
const string AnnotationAssFileReader::headerSubText = "Text";
const char   AnnotationAssFileReader::separator = ',';

AnnotationAssFileReader::AnnotationAssFileReader(int x_width, int x_height):
	m_inputWidth(x_width),
	m_inputHeight(x_height),
	m_widthProportion(0),
	m_heightProportion(0),
	m_idxStart(0),
	m_idxEnd(0),
	m_idxText(0)
{
}

AnnotationAssFileReader::~AnnotationAssFileReader()
{
}

void AnnotationAssFileReader::Open(const string& x_file)
{
	AnnotationFileReader::Open(x_file);
	InitReading();
}


Rect AnnotationAssFileReader::GetBox() const
{
	return m_boudingBox;
}

void AnnotationAssFileReader::ReadSrt(const string& srt)
{
	// {\clip(136,313,274,458)}fall

	// looking for "clip(", stop to next ")"
	string::size_type start = srt.find("clip(");
	string::size_type end = srt.find(")",start);
	if (start == string::npos || end == string::npos)
	{
		// no "clip" in srt, return the input text
		m_boudingBox = Rect();
		m_text = srt;
		return;
	}
	// split ","
	stringstream stream(srt.substr(start+ 5,end));
	int rect[4];
	string word;
	for (auto& elem : rect)
	{
		getline(stream, word, separator);
		elem = boost::lexical_cast<int>(word);
	}
	Point p1(rect[0] * m_widthProportion,rect[1] * m_heightProportion);
	Point p2(rect[2] * m_widthProportion,rect[3] * m_heightProportion);
	// save the rect
	m_boudingBox = Rect(p1,p2);

	// looking for "}", stop at end of the line, save the text
	start = srt.find("}",end);
	if (start != string::npos && start+1 <= srt.size())
		m_text = srt.substr(srt.find("}",end)+1);
}
void AnnotationAssFileReader::InitReading()
{
	string line;

	// looking for resolution
	ReadResolution();	// Warning: this function modify the ifstream, looking for a best solution

	// looking for srt header
	while(line.compare("[Events]") != 0)
	{
		SafeGetline(m_srtFile, line);
		if(! m_srtFile.good())
			throw MkException("End of file in AnnotationAssFileReader", LOC);
	}
	SafeGetline(m_srtFile, line);

	string::size_type pos = line.find("Start");
	if (pos == string::npos)
		throw MkException("Subtitle format error: must contain 'Start'", LOC);
	m_idxStart = count(line.begin(), line.begin() + pos, separator);

	pos = line.find("End");
	if (pos == string::npos)
		throw MkException("Subtitle format error: must contain 'End'", LOC);
	m_idxEnd = count(line.begin(), line.begin() + line.find("End"), separator);

	pos = line.find("Text");
	if (pos == string::npos)
		throw MkException("Subtitle format error: must contain 'Text'", LOC);
	m_idxText = count(line.begin(), line.begin() + line.find("Text"), separator);
}

bool AnnotationAssFileReader::ReadNextAnnotation(string& rx_subText)
{
	if(! m_srtFile.good())
	{
		LOG_DEBUG(m_logger, "End of subtitle file");
		rx_subText = "";
		return false;
	}

	string line;
	try
	{
		// skip blank lines
		while(line.size() == 0)
		{
			SafeGetline(m_srtFile, line);
			if(! m_srtFile.good())
				throw MkException("End of file in AnnotationAssFileReader", LOC);
		}

		// get start time
		stringstream stream(line);
		string word;
		for (int i = 0; i <= m_idxStart; i++)
		{
			getline(stream, word, separator);
		}

		FormatTimestamp(word);
		m_srtStart = word;

		// get end time
		stringstream stream2(line);
		for (int i = 0; i <= m_idxEnd; i++)
		{
			getline(stream2, word, separator);
		}
		FormatTimestamp(word);
		m_srtEnd = word;

		// get the text
		// statement: text is at the end of the file
		size_t pos = 0;
		for (int i = 0; i < m_idxText; i++)
		{
			pos = line.find(",", pos+1);
		}
		if (pos > line.size() || pos == string::npos)
		{
			m_srt = "";
			rx_subText = "";
		}
		else
		{
			m_srt = line.substr(pos+1);
			ReadSrt(m_srt);
			rx_subText = m_text;
		}

		LOG_DEBUG(m_logger, "Read next sub: "<<m_srt);
		return true;
	}
	catch(MkException& e)
	{
		LOG_ERROR(m_logger, "Exception while reading .ass file in AnnotationAssFileReader: " << e.what());
		rx_subText = "";
		m_srtStart = msToTimeStamp(0);
		m_srtEnd   = msToTimeStamp(0);
		return false;
	}
}

void AnnotationAssFileReader::FormatTimestamp(string& rx_timeText)
{
	// convert format (dot to comma for milisecond)
	string::size_type i = rx_timeText.find('.');
	if (i != string::npos)
		rx_timeText.replace(i++, 1, ",");

	// add a 0 if necessary
	i = rx_timeText.find(':');
	if (i == string::npos)
	{
		throw MkException("Subtitle format error: must contain ':'", LOC);
	}
	if (i == 1)
		rx_timeText = "0" + rx_timeText;
}

void AnnotationAssFileReader::ReadResolution()
{
	string line;
	while(line.find("PlayResX") != 0)
	{
		SafeGetline(m_srtFile, line);
		if(! m_srtFile.good())
			throw MkException("Subtitle format error: must contain 'PlayResX'", LOC);
	}
	string::size_type start = line.find_last_of(":");
	if (start == string::npos)
		throw MkException("Subtitle format error: must contain 'PlayResX : value'", LOC);
	int width = boost::lexical_cast<int>((line.substr(start+1)));
	SafeGetline(m_srtFile, line);
	if (line.find("PlayResY") == string::npos)
		throw MkException("Subtitle format error: must contain 'PlayResY'", LOC);
	start = line.find_last_of(":");
	if (start == string::npos)
		throw MkException("Subtitle format error: must contain 'PlayResY : value'", LOC);
	int height = boost::lexical_cast<int>((line.substr(start+1)));

	m_widthProportion  = static_cast<double>(m_inputWidth)  / width;
	m_heightProportion = static_cast<double>(m_inputHeight) / height;
}
