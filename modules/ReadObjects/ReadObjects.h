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
#include "AnnotationFileReader.h"


/**
* @brief Read a stream of objects and log data to a text file
*/
class ReadObjects : public Input
{

public:
	class Parameters : public Input::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Input::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterString("file"        , "in/objects.srt", &file      , "Name of the .srt file without extension"));
			m_list.push_back(new ParameterString("folder_name" , "events_img"    , &folder    , "Name of the folder to create for images"));
			m_list.push_back(new ParameterBool("one_frame_only", 0, 0, 1         , &oneFrameOnly, "Read only one frame, to define a fixed sets of objects"));

			RefParameterByName("type").SetDefault("CV_8UC3"); // This will probably be ignored
			Init();
		}
		std::string file;
		std::string folder;
		bool oneFrameOnly;
	};

	ReadObjects(ParameterStructure& xr_params);
	~ReadObjects(void);
	MKCLASS("ReadObjects")
	MKDESCR("Read an object from an annotation file")

	inline virtual const Parameters& GetParameters() const { return m_param;}
	void Reset();

private:
	inline virtual Parameters & RefParameters() {return m_param;}
	Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:

	virtual void Capture();

	// ouput
	std::vector<Object> m_ObjectOut;

	// temporary
	AnnotationFileReader* mp_annotationReader;
	std::string text;
	TIME_STAMP currentTimeStampTmp;
	bool firstAnnotation;

};

#endif

