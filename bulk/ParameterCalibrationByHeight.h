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

#ifndef PARAMETER_CALIBRATION_BY_HEIGHT_H
#define PARAMETER_CALIBRATION_BY_HEIGHT_H

#include "define.h"
#include "Parameter.h"
#include "CalibrationByHeight.h"

/// Parameter for special 3D calibration objects
class ParameterCalibrationByHeight : public Parameter
{
public:
	static CalibrationByHeight DefaultFg;
	static CalibrationByHeight DefaultBg;


public:
	ParameterCalibrationByHeight(const std::string& x_name, const CalibrationByHeight& x_default, CalibrationByHeight * xp_value, const std::string& x_description) :
		Parameter(x_name, x_description),
		m_default(x_default),
		mp_value(xp_value)
	{
	}
	inline std::string GetValueString() const {
		if (mp_value->x == 0 && mp_value->y == 0 && mp_value->heigth == 0)
			return "";
		std::stringstream ss;
		mp_value->Serialize(ss,"");
		return ss.str();
	}
	inline std::string GetDefaultString() const{std::stringstream ss; m_default.Serialize(ss,""); return ss.str();}
	inline std::string GetRange() const{/*std::stringstream ss; ss<<"["<<m_min<<":"<<m_max<<"]"; return ss.str();*/ return "";}	
	inline const ParameterType& GetType() const {const static ParameterType s = PARAM_OBJECT_HEIGHT; return s;}
	inline const std::string& GetTypeString() const {const static std::string s = "objectHeigth"; return s;}
	inline const CalibrationByHeight& GetDefault() const {
		return m_default;}

	inline void SetValue(const CalibrationByHeight& x_value, ParameterConfigType x_confType)
	{
		if(m_isLocked)
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		*mp_value = x_value;
		m_confSource = x_confType;
	}

	virtual void SetValue(const std::string& rx_value, ParameterConfigType x_confType /*= PARAMCONF_UNKNOWN*/)
	{
		if(m_isLocked)
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		std::istringstream istr(rx_value);
		if(rx_value == "")	// This case happens with unit testing
		{
			mp_value->x = 0;
			mp_value->y = 0;
			mp_value->heigth = 0;
			return;
		}

		mp_value->Deserialize(istr, ""); // atof is sensible to locale format and may use , as a separator
		m_confSource = x_confType;
	}
	virtual void SetDefault(const std::string& rx_value)
	{
		std::istringstream istr(rx_value);
		m_default.Deserialize(istr); // atof is sensible to locale format and may use , as a separator
	}
	const CalibrationByHeight& GetValue() const
	{
		return *mp_value;
	}
	virtual bool CheckRange() const
	{
		CalibrationByHeight object = GetValue();
		return ((object.x <= 1 && object.x >= 0) &&
		(object.y <= 1 && object.y >= 0) &&
		(object.heigth <= 1 && object.heigth >= 0));
	}
	virtual void Print(std::ostream& os) const
	{
		os<<m_name<<" : x = "<< mp_value->x
				  <<", y = "<< mp_value->y
				  <<", heigth = "<< mp_value->heigth
				  <<" ("<<configType[m_confSource]<<"); ";
	}
	virtual void SetValueToDefault()
	{
		if(m_isLocked)
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		*mp_value = m_default;
		m_confSource = PARAMCONF_DEF;
	}
	virtual void Export(std::ostream& rx_os, int x_indentation);

private:
	CalibrationByHeight m_default;
	CalibrationByHeight* mp_value;
};


#endif
