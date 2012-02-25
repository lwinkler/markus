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

#ifndef INPUT_H
#define INPUT_H

#include <cv.h>

#include "ConfigReader.h"
#include "Parameter.h"
#include "Module.h"


class InputParameterStructure : public ModuleParameterStructure
{
public:
	InputParameterStructure(const ConfigReader& x_confReader) : ModuleParameterStructure(x_confReader)
	{
	}
};

class Input : public Module
{
public:
	Input(const ConfigReader& x_confReader);
	~Input();
	
	virtual void Capture() = 0;
	inline const std::string& GetName()const {return m_name;};
	virtual const cv::Mat * GetImage() const = 0;
	
	inline double GetFps() {return RefParameter().fps;};
	
	inline void ProcessFrame() {Capture();};
	
	virtual inline bool IsInput() {return true;};
private:

protected:
	const std::string m_name;
        //int m_width;
        //int m_height;
	
	virtual InputParameterStructure& RefParameter() = 0;
};

#endif
