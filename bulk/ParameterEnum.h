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

#ifndef PARAMETER_ENUM_H
#define PARAMETER_ENUM_H

#include "define.h"
#include "Parameter.h"
#include "MkException.h"
#include <log4cxx/logger.h>

/// Parameter of type enum
class ParameterEnum : public Parameter
{
public:
	ParameterEnum(const std::string& x_name, int x_default, int * xp_value, const std::string x_description):
		Parameter(x_name, x_description),
		m_default(x_default),
		mp_value(xp_value){}
	void SetValue(const std::string& rx_value, ParameterConfigType x_confType/* = PARAMCONF_UNKNOWN*/);
	void SetValue(int rx_value, ParameterConfigType x_confType/* = PARAMCONF_UNKNOWN*/);
	void SetDefault(const std::string& rx_value);
	inline int GetDefault() const {return m_default;}
	inline int GetValue() const{return *mp_value;}
	inline std::string GetValueString() const {return GetReverseEnum().at(*mp_value);}
	inline std::string GetDefaultString() const{return GetReverseEnum().at(m_default);}
	inline std::string GetRange() const
	{
		std::stringstream ss; 
		for(std::map<std::string,int>::const_iterator it = GetEnum().begin() ; it != GetEnum().end() ; it++)
			ss<<it->first<<",";
		return ss.str();
	}
	virtual bool CheckRange() const;
	virtual void Print(std::ostream& os) const;
	virtual void SetValueToDefault()
	{
		if(m_isLocked) 
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		*mp_value = m_default;
		m_confSource = PARAMCONF_DEF;
	}
	inline const ParameterType& GetType() const {const static ParameterType s = PARAM_IMAGE_TYPE; return s;}
	virtual const std::string& GetTypeString() const = 0;
	virtual void Export(std::ostream& rx_os, int x_indentation);
	virtual const std::map<std::string, int>& GetEnum() const = 0;
	virtual const std::map<int, std::string>& GetReverseEnum() const = 0;
	
protected:

	int m_default;
	int* mp_value;
};

/// Parameter for the type of an OpenCV image
class ParameterImageType : public ParameterEnum
{
public:
	ParameterImageType(const std::string& x_name, int x_default, int * xp_value, const std::string x_description);
	~ParameterImageType(){}
	// void Export(std::ostream& rx_os, int x_indentation);

	// Conversion methods
	inline const std::string& GetTypeString() const {const static std::string s = "imageType"; return s;}
	const std::map<std::string, int> & GetEnum() const {return Enum;}
	const std::map<int, std::string> & GetReverseEnum() const {return ReverseEnum;}
	static std::map<std::string, int> CreateMap();
	static std::map<int, std::string> CreateReverseMap();

	// static attributes
	const static std::map<std::string, int> Enum;
	const static std::map<int, std::string> ReverseEnum;
};
#endif
