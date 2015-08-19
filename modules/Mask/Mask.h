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

#ifndef MASK_H
#define MASK_H

#include "Module.h"
#include "opencv2/video/background_segm.hpp"


/**
* @brief Apply a binary mask to an image input
*/
class Mask : public Module
{
public:
	class Parameters : public Module::Parameters
	{
	public:
		Parameters(const ConfigReader& x_confReader) : Module::Parameters(x_confReader)
		{
			RefParameterByName("type").SetRange("[CV_8UC1,CV_8UC3]");
			Init();
		};
	};

	Mask(ParameterStructure& xr_params);
	~Mask();
	MKCLASS("Mask")
	MKCATEG("Image")
	MKDESCR("Apply a binary mask to an image input")

	virtual void ProcessFrame();
	void Reset();

private:
	const Parameters& m_param;
	static log4cxx::LoggerPtr m_logger;

protected:

	// input
	cv::Mat m_input;
	cv::Mat m_mask;

	// output
	cv::Mat m_output;
};


#endif
