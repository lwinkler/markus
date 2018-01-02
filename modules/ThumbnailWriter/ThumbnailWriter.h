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




namespace mk {
/**
* @brief Write all objects as a collection of images (thumbnails)
*/
class ThumbnailWriter : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			AddParameter(new ParameterString("folder"    , "thumbs" , &folder    , "Name of the folder to create with path. Use %{feature} to separate by feature"));
			AddParameter(new ParameterString("extension"  , "jpg"        , &extension , "Extension of the thumbnails. Determines the output format."));

			RefParameterByName("type").SetRange(R"({"allowed":["CV_8UC1","CV_8UC3","CV_32FC1","CV_32FC3"]})"_json);
			RefParameterByName("extension").SetRange(R"({"allowed":["jpg","png"]})"_json);
		};
		std::string folder;
		std::string extension;
	};

	explicit ThumbnailWriter(ParameterStructure& xr_params);
	virtual ~ThumbnailWriter();
	MKCLASS("ThumbnailWriter")
	MKCATEG("Output")
	MKDESCR("Write all objects as a collection of images (thumbnails)")

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	void ProcessFrame() override;
	void Reset() override;

	// input
	cv::Mat m_input;
	cv::Mat m_input2;
	std::vector <Object> m_objectsIn;
};

} // namespace mk
#endif
