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

#ifndef PARAMETER_NUM_H
#define PARAMETER_NUM_H

#include "define.h"
#include "MkException.h"
#include "Parameter.h"

/// Template for all parameters with numerical values
template<class T> class ParameterNum : public Parameter
{
public:
	ParameterNum(const std::string& x_name, T x_default, T x_min, T x_max, T * xp_value, const std::string& x_description) : 
		Parameter(x_name, x_description),
		m_default(x_default),
		m_min(x_min),
		m_max(x_max),
		mr_value(*xp_value){}
	inline std::string GetValueString() const {std::stringstream ss; ss<<mr_value; return ss.str();}
	inline std::string GetDefaultString() const{std::stringstream ss; ss<<m_default; return ss.str();}
	inline std::string GetRange() const{std::stringstream ss; ss<<"["<<m_min<<":"<<m_max<<"]"; return ss.str();}
	inline const ParameterType& GetType() const {return m_type;}
	inline const std::string& GetTypeString() const{return m_typeStr;}
	inline const T GetDefault() const {return m_default;}
	inline const T GetMin() const {return m_min;}
	inline const T GetMax() const{return m_max;}
	
	virtual void SetValue(const std::string& rx_value, ParameterConfigType x_confType/* = PARAMCONF_UNKNOWN*/)
	{
		if(m_isLocked) 
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		std::istringstream istr(rx_value);
		istr >> mr_value; // atof is sensible to locale format and may use , as a separator
		m_confSource = x_confType;
	}
	inline void SetValue(T x_value, ParameterConfigType x_confType/* = PARAMCONF_UNKNOWN*/)
	{
		if(m_isLocked) 
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		mr_value = x_value;
		m_confSource = x_confType;
	}
	/*virtual void SetValue(const void * px_value, ParameterConfigType x_confType = PARAMCONF_UNKNOWN)
	{	
		mr_value = *static_cast<const T*>(px_value);
		m_confSource = x_confType;
	};*/
	virtual void SetDefault(const std::string& rx_value)
	{	
		std::istringstream istr(rx_value);
		istr >> m_default; // atof is sensible to locale format and may use , as a separator
	}
	virtual T GetValue() const
	{
		return mr_value;
	}
	virtual bool CheckRange() const
	{
		T value = GetValue();
		return (value <= m_max && value >= m_min);
	}
	virtual void Print(std::ostream& os) const 
	{
		os<<m_name<<" = "<<GetValue()<<" ["<<m_min<<":"<<m_max<<"] ("<<configType[m_confSource]<<"); ";
	}
	virtual void SetValueToDefault()
	{
		if(m_isLocked) 
			throw MkException("You tried to set the value of a locked parameter.", LOC);
		mr_value = m_default;
		m_confSource = PARAMCONF_DEF;
	}
	virtual void Export(std::ostream& rx_os, int x_indentation)
	{
		std::string tabs(x_indentation, '\t');
		rx_os<<tabs<<"<param name=\""<<m_name<<"\">"<<std::endl;
		tabs = std::string(x_indentation + 1, '\t');
		rx_os<<tabs<<"<type>"<<GetTypeString()<<"</type>"<<std::endl;
		rx_os<<tabs<<"<value min=\""<<m_min<<"\" max=\""<<m_max<<"\" default=\""<<m_default<<"\">"<<GetValue()<<"</value>"<<std::endl;
		rx_os<<tabs<<"<description>"<<m_description<<"</description>"<<std::endl;
		tabs = std::string(x_indentation, '\t');
		rx_os<<tabs<<"</param>"<<std::endl;
	}
	
	T m_default;
	const T m_min;
	const T m_max;
private:
	T& mr_value;
	static const ParameterType m_type;
	static const std::string m_typeStr;
};



/// Parameter of type integer
typedef ParameterNum<int> 	ParameterInt;
/// Parameter of type double
typedef ParameterNum<double> 	ParameterDouble;
/// Parameter of type float
typedef ParameterNum<float> 	ParameterFloat;
/// Parameter of type boolean
typedef ParameterNum<bool> 	ParameterBool;

#endif