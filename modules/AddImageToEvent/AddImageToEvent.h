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

#ifndef ADD_IMAGE_TO_EVENT_H
#define ADD_IMAGE_TO_EVENT_H

#include "Module.h"
#include "Event.h"




/**
* @brief Add an attached image to an event (after saving it as a file)
*/
class AddImageToEvent : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{
			AddParameter(new ParameterString("folder_name" ,  "events_img"   , &folder    ,  "Name of the folder to create for images"));
			AddParameter(new ParameterString("extension"  , "jpg"        , &extension , "Extension of the thumbnails. Determines the output format."));

			RefParameterByName("type").SetRange("[CV_8UC1,CV_8UC3,CV_32FC1,CV_32FC3]");
			RefParameterByName("extension").SetRange("[jpg,png]");
		};
		std::string folder;
		std::string extension;
	};

	AddImageToEvent(ParameterStructure& xr_params);
	virtual ~AddImageToEvent();
	MKCLASS("AddImageToEvent")
	MKCATEG("Output")
	MKDESCR("Add an attached image to an event (after saving it as a file)")

	virtual void ProcessFrame() override;
	virtual void Reset() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	cv::Mat m_inputIm1;
	// cv::Mat m_inputIm2;
	Event   m_event;

	// temporary
	bool m_saveImage1 = false;
};

#endif
