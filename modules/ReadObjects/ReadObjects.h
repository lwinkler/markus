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

#ifndef READ_OBJECTS_H
#define READ_OBJECTS_H

#include "Input.h"
#include "StreamObject.h"
#include <fstream>


class AnnotationFileReader;

/**
* @brief Read a stream of objects and log data to a text file
*/
class ReadObjects : public Input
{

public:
	class Parameters : public Input::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Input::Parameters(x_name)
		{
			AddParameter(new ParameterString("file"        , "in/objects.srt", &file      , "Name of the .srt file without extension"));

			RefParameterByName("type").SetDefaultAndValue("CV_8UC3"); // This will probably be ignored
		}
		std::string file;
	};
	bool AbortCondition() const override {return m_endOfStream;}

	explicit ReadObjects(ParameterStructure& xr_params);
	virtual ~ReadObjects();
	MKCLASS("ReadObjects")
	MKCATEG("Input")
	MKDESCR("Read an object from an annotation file")

	void Reset() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:

	void Capture() override;

	// ouput
	std::vector<Object> m_ObjectOut;

	// temporary
	AnnotationFileReader* mp_annotationReader = nullptr;
};

#endif

