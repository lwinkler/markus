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

#ifndef PARAMETER_CONTROL_H
#define PARAMETER_CONTROL_H

#include <string>
#include <vector>
#include "Control.h"


class ParameterControl : public Control
{
public:
	ParameterControl(const std::string& x_name, const std::string& x_description);
	~ParameterControl();
	void SetParameterStructure(ParameterStructure& rx_param);
	void CleanParameterStructure();
	virtual void Destroy(){CleanParameterStructure();};
private:
	ParameterStructure* mp_param;
};

#endif
