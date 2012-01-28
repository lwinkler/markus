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

#include <cv.h>
#include "Module.h"
#include "Parameter.h"


/*! \class SlitCam
 *  \brief Class containing methods/attributes of a slit camera
 *
 */


class SlitCamParameter : public ModuleParameterStructure
{
	
public:
	SlitCamParameter(const ConfigReader& x_confReader) : 
		ModuleParameterStructure(x_confReader)
	{
		m_list.push_back(new ParameterInt(0, "aperture", 1, PARAM_INT, 1, 10, &aperture,	"Size of the aperture of the camera"));
		
		ParameterStructure::Init();
	};
	
	int aperture;
};

class SlitCam : public Module
{
private:
	SlitCamParameter m_param;
	int m_position;
	static const char * m_type;
	
	cv::Mat * m_input;
	cv::Mat * m_output;
public:
	SlitCam(const ConfigReader& x_configReader);
	~SlitCam(void);
	//void CreateParamWindow();
	
	virtual void ProcessFrame();
	inline virtual int GetInputWidth() const {return m_param.width;};
	inline virtual int GetInputHeight() const {return m_param.height;};

protected:
	inline virtual const ModuleParameterStructure& GetRefParameter() const { return m_param;};
};

#endif

