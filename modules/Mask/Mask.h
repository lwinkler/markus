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


class ConfigReader;


class MaskParameterStructure : public ModuleParameterStructure
{
public:
	MaskParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
		Init();
	};
};

/**
* @brief Apply a binary mask to an image input
*/
class Mask : public Module
{
public:
	Mask(const ConfigReader& x_configReader);
	~Mask();
	MKCLASS("Mask")
	MKDESCR("Apply a binary mask to an image input")

	
	virtual void ProcessFrame();
	void Reset();
private:
	MaskParameterStructure m_param;
	inline virtual const ModuleParameterStructure& GetParameters() const { return m_param;}
	static log4cxx::LoggerPtr m_logger;
protected:

	// input
	cv::Mat m_input;
	cv::Mat m_mask;

	// output
	cv::Mat m_output;
};


#endif
