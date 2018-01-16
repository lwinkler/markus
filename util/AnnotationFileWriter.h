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

#ifndef ANNOTATION_FILE_WRITER_H
#define ANNOTATION_FILE_WRITER_H

#include <log4cxx/logger.h>
#include <fstream>
#include "define.h"


namespace mk {
/**
* @brief Read an annotation file (in .srt format)
*/
class AnnotationFileWriter
{
public:
	AnnotationFileWriter();
	virtual ~AnnotationFileWriter();

	void Open(const std::string& x_file);
	void WriteAnnotation(TIME_STAMP x_start, TIME_STAMP x_end, std::stringstream& x_in);

private:
	static log4cxx::LoggerPtr m_logger;

protected:
	int m_subId;
	std::ofstream m_file;
};

} // namespace mk
#endif
