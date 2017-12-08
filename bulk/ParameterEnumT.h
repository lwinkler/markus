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

#ifndef PARAMETER_ENUM_T_H
#define PARAMETER_ENUM_T_H

#include "define.h"
#include "ParameterEnum.h"
#include "MkException.h"

/// Parameter of type enum
template<class T> class ParameterEnumT : public ParameterEnum
{
public:
	ParameterEnumT(const std::string& x_name, int x_default, int * xp_value, const std::string& x_description):
		ParameterEnum(x_name, x_default, xp_value, x_description)
	{}
	
	const std::map<std::string, int>& GetEnum() const override {return Enum;}
	const std::map<int, std::string>& GetReverseEnum() const override {return ReverseEnum;}
	const std::string& GetClass() const override {return className;};
	static const std::string className;

protected:
	static const std::map<std::string, int> Enum;
	static const std::map<int, std::string>  ReverseEnum;
};

#endif
