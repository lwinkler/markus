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

#ifndef THUMBNAIL_WRITER_H
#define THUMBNAIL_WRITER_H

#include "Module.h"
#include "StreamObject.h"




/**
* @brief Write all objects as a collection of images (thumbnails)
*/
class ThumbnailWriter : public Module
{
public:
	class Parameters : public Module::Parameters
	{
		public:
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterString("folder"    , "thumbs" , &folder    , "Name of the folder to create with path. Use %{feature} to separate by feature"));
			m_list.push_back(new ParameterString("extension"  , "jpg"        , &extension , "Extension of the thumbnails. Determines the output format."));

			RefParameterByName("type").SetRange("[CV_8UC1,CV_8UC3,CV_32FC1,CV_32FC3]");
			Init();
		};
		std::string folder;
		std::string extension;
	};

	ThumbnailWriter(const ConfigReader& x_confReader);
	~ThumbnailWriter();
	MKCLASS("ThumbnailWriter")
	MKDESCR("Write all objects as a collection of images (thumbnails)")
	
	inline virtual const Parameters& GetParameters() const {return m_param;}
	virtual void ProcessFrame();
	void Reset();

private:
	inline virtual Parameters & RefParameters() {return m_param;}
	Parameters m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	// input
	cv::Mat m_input;
	cv::Mat m_input2;
	std::vector <Object> m_objectsIn;

	// temporary
	std::string m_folderName;
};

#endif
