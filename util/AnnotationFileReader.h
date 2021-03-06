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

#ifndef ANNOTATION_FILE_READER_H
#define ANNOTATION_FILE_READER_H

#include <log4cxx/logger.h>
#include <fstream>
#include "define.h"
#include "Module.h"


namespace mk {
/**
* @brief Read an annotation file
*/
class AnnotationFileReader
{
public:
	AnnotationFileReader();
	virtual ~AnnotationFileReader();

	virtual void Open(const std::string& x_file);
	TIME_STAMP GetCurrentTimeStamp();
	TIME_STAMP GetEndTimeStamp();
	virtual bool ReadNextAnnotation(std::string& rx_subText) = 0;
	virtual std::string ReadAnnotationForTimeStamp(TIME_STAMP x_current);
	virtual cv::Rect GetBox() const = 0;

protected:
	std::istream& SafeGetline(std::istream& is, std::string& t);

private:
	static log4cxx::LoggerPtr m_logger;

protected:
	int m_num;
	std::string m_text;
	std::ifstream m_srtFile;
	std::string m_srtStart;
	std::string m_srtEnd;
};

} // namespace mk
#endif
