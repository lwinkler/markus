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

// #include <opencv2/highgui/highgui.hpp>
#include <Module.h>
#include <StreamObject.h>



class ThumbnailWriterParameterStructure : public ModuleParameterStructure
{
public:
	ThumbnailWriterParameterStructure(const ConfigReader& x_confReader) : 
	ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterString("folder"     , "thumbs" , &folder    , "Name of the folder to create, with path"));
		m_list.push_back(new ParameterString("extension"  , "jpg"        , &extension , "Extension of the thumbnails. Determines the output format."));
		ParameterStructure::Init();
	};

	std::string folder;
	std::string extension;
};

class ThumbnailWriter : public Module
{
public:
	ThumbnailWriter(const ConfigReader& x_confReader);
	~ThumbnailWriter();
	
	virtual void ProcessFrame();
	void Reset();
private:
	ThumbnailWriterParameterStructure m_param;
	inline virtual const ThumbnailWriterParameterStructure& RefParameter() const {return m_param;};
protected:
	cv::Mat m_input;
	std::vector <Object> m_objectsIn;
	std::string m_folderName;
};

#endif
