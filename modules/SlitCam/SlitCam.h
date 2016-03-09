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

#ifndef SLITCAM_H
#define SLITCAM_H

#include "Module.h"
#include "Parameter.h"


/**
* @brief A simple example module that mimics a slit camera (or linear camera). The camera input is a range of pixels in the middle of the image.
*/
class SlitCam : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) :
			Module::Parameters(x_confReader)
		{
			m_list.push_back(new ParameterInt("aperture", 1, 1, 10, &aperture,	"Size of the aperture of the camera"));

			RefParameterByName("width").SetRange("[20:6400]");
			RefParameterByName("height").SetRange("[20:4800]");
			RefParameterByName("type").SetDefault("CV_8UC3");

			Init();
		}

		int aperture;
	};

	SlitCam(ParameterStructure& xr_params);
	~SlitCam(void);
	MKCLASS("SlitCam")
	MKCATEG("Input")
	MKDESCR("A simple example module that mimics a slit camera (or linear camera). The camera input is a range of pixels in the middle of the image.")
	virtual void ProcessFrame() override;
	virtual void Reset() override;

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:

	// input
	cv::Mat m_input;

	// output
	cv::Mat m_output;

	// state
	int m_position;
};

#endif

