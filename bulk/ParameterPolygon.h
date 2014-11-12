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

// TODO: Unit test this

#ifndef PARAMETER_POLYGON_H
#define PARAMETER_POLYGON_H

#include "define.h"
#include "Parameter.h"
#include "Polygon.h"

class ParameterPolygon : public Parameter
{

public:
	static Polygon DefaultNull;

public:
	ParameterPolygon(const std::string& x_name, const Polygon& x_default, Polygon * xp_value, const std::string& x_description) :
		Parameter(x_name, x_description),
		m_default(x_default),
		mr_value(*xp_value)
	{
	}
	inline std::string GetValueString() const
	{
		if (mr_value.m_value.size() == 0)
			return ""; // Note: this is for testing purpose
		return mr_value.SerializeToString();
	}
	inline std::string GetDefaultString() const{std::stringstream ss; m_default.Serialize(ss,""); return ss.str();}
	inline std::string GetRange() const{return "";}
	inline virtual void SetRange(const std::string& x_range){}
	inline const ParameterType& GetType() const {const static ParameterType s = PARAM_GENERIC; return s;}
	inline const std::string& GetTypeString() const {const static std::string s = "Polygon"; return s;}
	inline const Polygon& GetDefault() const {
		return m_default;}

	inline void SetValue(const Polygon& x_value, ParameterConfigType x_confType)
	{
		if(m_isLocked)
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		mr_value = x_value;
		m_confSource = x_confType;
	}

	virtual void SetValue(const std::string& rx_value, ParameterConfigType x_confType /*= PARAMCONF_UNKNOWN*/)
	{
		if(m_isLocked)
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		std::istringstream istr(rx_value);
		if(rx_value == "")	// This case happens with unit testing
		{
			return;
		}

		mr_value.Deserialize(istr, ""); // atof is sensible to locale format and may use , as a separator
		m_confSource = x_confType;
	}
	virtual void SetDefault(const std::string& rx_value)
	{
		std::istringstream istr(rx_value);
		m_default.Deserialize(istr); // atof is sensible to locale format and may use , as a separator
	}
	const Polygon& GetValue() const
	{
		return mr_value;
	}
	virtual bool CheckRange() const
	{
		return true;
	}
	virtual void GenerateValues(int x_nbSamples, std::vector<std::string>& rx_values, const std::string& x_range = "") const;
	virtual void SetValueToDefault()
	{
		if(m_isLocked)
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		mr_value = m_default;
		m_confSource = PARAMCONF_DEF;
	}
	virtual void Export(std::ostream& rx_os, int x_indentation);

private:
	Polygon m_default;
	Polygon& mr_value;
};


#endif
