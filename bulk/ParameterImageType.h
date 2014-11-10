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

#ifndef PARAMETER_IMAGE_TYPE_H
#define PARAMETER_IMAGE_TYPE_H

#include "define.h"
#include "ParameterEnum.h"


/// Parameter for the type of an OpenCV image
class ParameterImageType : public ParameterEnum
{
public:
	ParameterImageType(const std::string& x_name, int x_default, int * xp_value, const std::string& x_description);
	~ParameterImageType(){}
	// void Export(std::ostream& rx_os, int x_indentation);

	// Conversion methods
	inline const std::string& GetTypeString() const {const static std::string s = "imageType"; return s;}
	const std::map<std::string, int> & GetEnum() const {return Enum;}
	const std::map<int, std::string> & GetReverseEnum() const {return ReverseEnum;}
	static std::map<std::string, int> CreateMap();
	static std::map<int, std::string> CreateReverseMap(const std::map<std::string, int>& x_map);

	// static attributes
	const static std::map<std::string, int> Enum;
	const static std::map<int, std::string> ReverseEnum;
};
#endif
