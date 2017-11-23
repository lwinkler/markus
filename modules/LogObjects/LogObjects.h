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

#ifndef LOG_OBJECTS_H
#define LOG_OBJECTS_H

#include "Module.h"
#include "StreamObject.h"
#include <fstream>
#include "AnnotationFileWriter.h"


/**
* @brief Read a stream of objects and log data to a text file
*/
class LogObjects : public Module
{

public:
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			AddParameter(new ParameterString("file"   , "objects.%d.srt" , &file , "Name of the .srt file without extension"));
			AddParameter(new ParameterBool("compress" , 0 , 0     , 1    , &compress , "Compress the result as a tar.gz"));
		}
		std::string file;
		bool compress;
	};

	explicit LogObjects(ParameterStructure& xr_params);
	virtual ~LogObjects();
	MKCLASS("LogObjects")
	MKCATEG("Output")
	MKDESCR("Read a stream of objects and log data to a text file")

	void ProcessFrame() override;
	void Reset() override;
	void Compress();

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	std::vector <Object> m_objectsIn;

	// temporary
	std::ofstream m_outputFile;
	AnnotationFileWriter* mp_annotationWriter;
};

#endif

