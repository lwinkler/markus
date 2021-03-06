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

#ifndef RENDER_OBJECTS_H
#define RENDER_OBJECTS_H

#include "Module.h"


namespace mk {
class Object;

/**
* @brief Output video stream with additional object streams
*/
class RenderObjects : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		explicit Parameters(const std::string& x_name) : Module::Parameters(x_name)
		{
			RefParameterByName("type").SetDefaultAndValue("CV_8UC3");
		};
	};

	explicit RenderObjects(ParameterStructure& xr_params);
	~RenderObjects() override;
	MKCLASS("RenderObjects")
	MKDESCR("Output video stream with additional object streams")

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:
	void Reset() override;
	void ProcessFrame() override;

	// input
	cv::Mat m_imageInput;
	std::vector<std::vector<Object>> m_objectInputs;

	// output
	cv::Mat m_imageOutput;
};


} // namespace mk
#endif
