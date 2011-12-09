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

#ifndef FACE_DETECTOR_H
#define FACE_DETECTOR_H

#include <cv.h>
#include "ModuleAsync.h"
#include "Parameter.h"


/*! \class FaceDetector
 *  \brief Class containing methods/attributes of a slit camera
 *
 */


class FaceDetectorParameter : public ModuleParameterStructure
{
	
public:
	FaceDetectorParameter(ConfigReader& x_confReader, const std::string& x_moduleName) : 
		ModuleParameterStructure(x_confReader, x_moduleName)
	{
		//m_list.push_back(new ParameterT<int>(0, "aperture", 1, PARAM_INT, 1, 10, &aperture));
		
		ParameterStructure::Init();
	};
	
	//int aperture;
};

class FaceDetector : public ModuleAsync
{
private:
	FaceDetectorParameter m_param;
	static const char * m_type;
public:
	FaceDetector(const std::string& x_name, ConfigReader& x_configReader);
	~FaceDetector(void);
	//void CreateParamWindow();
	
	virtual void ThreadProcess(const IplImage * img, const double x_timeSinceLastProcessing);

protected:
};

#endif

