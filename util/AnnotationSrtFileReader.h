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

#ifndef ANNOTATION_SRT_FILE_READER_H
#define ANNOTATION_SRT_FILE_READER_H

#include <fstream>
#include <log4cxx/logger.h>
#include "define.h"
#include "AnnotationFileReader.h"


/**
* @brief Read an annotation file (in .srt format)
*/
class AnnotationSrtFileReader : public AnnotationFileReader
{
public:
	AnnotationSrtFileReader();
	virtual ~AnnotationSrtFileReader();
	bool ReadNextAnnotation(std::string& rx_subText) override;

	/// Cannot return a box since we do not have this info in .srt files
	virtual cv::Rect GetBox() const override {return cv::Rect();}

private:
	static log4cxx::LoggerPtr m_logger;

};

#endif
