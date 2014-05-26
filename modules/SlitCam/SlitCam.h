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
			
			// Set default value of module parameter (re-definition)
			RefParameterByName("type").SetDefault("CV_8UC3");

			Init();
		}
		
		int aperture;
	};

	SlitCam(const ConfigReader& x_configReader);
	~SlitCam(void);
	MKCLASS("SlitCam")
	MKDESCR("A simple example module that mimics a slit camera (or linear camera). The camera input is a range of pixels in the middle of the image.")
	virtual void ProcessFrame();
	void Reset();

private:
	Parameters m_param;
	inline virtual const Parameters& GetParameters() const { return m_param;}
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

