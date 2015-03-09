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

#ifndef ANNOTATION_ASS_FILE_READER_H
#define ANNOTATION_ASS_FILE_READER_H

#include <fstream>
#include <log4cxx/logger.h>
#include "define.h"
#include "AnnotationFileReader.h"

/**
* @brief Read an annotation file (in .ass format)
*/
class AnnotationAssFileReader : public AnnotationFileReader
{
public:
	AnnotationAssFileReader(int x_width, int x_height);
	~AnnotationAssFileReader();
	bool ReadNextAnnotation(std::string& rx_subText);
	void Open(const std::string& x_file);
	cv::Rect GetBox();

protected:
	double m_widthProportion;
	double m_heightProportion;

	static const std::string headerStartTime;
	static const std::string headerEndTime;
	static const std::string headerSubText;
	static const char separator;

private:
	static log4cxx::LoggerPtr m_logger;
	void ReadSrt(const std::string& srt);
	void InitReading();
	void FormatTimestamp(std::string& rx_timeText);
	void ReadResolution();
	int m_idxStart;
	int m_idxEnd;
	int m_idxText;
	int m_inputWidth;
	int m_inputHeight;
	cv::Rect m_boudingBox;
	std::string m_srt;



};

#endif
