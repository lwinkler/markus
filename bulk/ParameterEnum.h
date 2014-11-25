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

/// Parameter of type enum
class ParameterEnum : public Parameter
{
public:
	ParameterEnum(const std::string& x_name, int x_default, int * xp_value, const std::string& x_description):
		Parameter(x_name, x_description),
		m_default(x_default),
		mr_value(*xp_value),
		m_allowAllValues(true){}
	void SetValue(const std::string& rx_value, ParameterConfigType x_confType/* = PARAMCONF_UNKNOWN*/);
	void SetValue(int rx_value, ParameterConfigType x_confType/* = PARAMCONF_UNKNOWN*/);
	void SetDefault(const std::string& rx_value);
	inline int GetDefault() const {return m_default;}
	inline int GetValue() const{return mr_value;}
	inline std::string GetValueString() const {return GetReverseEnum().at(mr_value);}
	inline std::string GetDefaultString() const{return GetReverseEnum().at(m_default);}
	std::string GetRange() const;
	virtual void SetRange(const std::string& x_range);
	inline void AllowAllValues(bool x_allow){m_allowAllValues = x_allow;}
	inline void AllowValue(const std::string& x_value, bool x_allow){m_allowedValues[GetEnum().at(x_value)] = x_allow;}
	virtual bool CheckRange() const;
	virtual void GenerateValues(int x_nbSamples, std::vector<std::string>& rx_values, const std::string& x_range = "") const;
	virtual void Print(std::ostream& os) const;
	virtual void SetValueToDefault()
	{
		if(m_isLocked) 
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		mr_value = m_default;
		m_confSource = PARAMCONF_DEF;
	}
	inline const ParameterType& GetType() const {const static ParameterType s = PARAM_IMAGE_TYPE; return s;}
	virtual const std::string& GetTypeString() const = 0;
	virtual void Export(std::ostream& rx_os, int x_indentation);
	virtual const std::map<std::string, int>& GetEnum() const = 0;
	virtual const std::map<int, std::string>& GetReverseEnum() const = 0;
	
protected:
	static std::map<int, std::string> CreateReverseMap(const std::map<std::string, int>& x_map);

	bool m_allowAllValues;
	std::map<int, bool> m_allowedValues;

	int m_default;
	int& mr_value;
};

#endif
